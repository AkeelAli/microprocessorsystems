#include "gesture_latch.h"


/* global variables	 */
LSM_Acc_ConfigTypeDef accConfig;
NVIC_InitTypeDef nvicConfig;
TIM_TimeBaseInitTypeDef timConfig;

/*	arrays for raw Accelerometer and Gyroscope 
	need 4 slots in each of the 3D buffers for 
	filtering */ 
//raw arrays for Accelerometer and Magnetometer
s16 rawAcc[4][3];		//4 buffers of 3 s16 values each
s16 rawGyro[4][3];

/*storage circular buffers for statistics */
//history of pitch & roll (and their squares)
float pitch_stat[10], roll_stat[10], pitch2_stat[10], roll2_stat[10];
float mean_pitch, mean_roll, pitchsquare, rollsquare, var_pitch, var_roll;

short stat_count = 0;
short newData = 0;	 //set when there is new raw data after a TIM2 interrupt
int data = 0; //counter for raw data placement in buffer (counts up to 3 then wraps back to 0)

float accelX,accelY,accelZ;
float accelYZ,accelXY;
float roll, pitch,yaw,roll_fuse,yaw_fuse,pitch_fuse;
float gyrosp_yaw_b, gyro_yaw, gyrosp_yaw;
float gyrosp_roll_b, gyro_roll, gyrosp_roll;
float gyrosp_pitch_b, gyro_pitch, gyrosp_pitch;
float fuse_factor_r,fuse_factor_p;
float ax_project,ay_project;

//UpdateGesture stores the move in this variable
symbol_t move; 

uint16_t readout=0;

void initTIM(void){
	//ENABLE CLOCK FOR TIM 2's CLOCK
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3,ENABLE);
	//Set up the timer
	timConfig.TIM_Period = 80;
	timConfig.TIM_Prescaler = 999; //Want to trigger every 10ms 8Meg/(80*(999+1)) = 100 (100Hz)
	timConfig.TIM_CounterMode = TIM_CounterMode_Up;
  	timConfig.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM2,&timConfig);

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//enable the Timer interrupt

	TIM_ARRPreloadConfig(TIM2,ENABLE);


	timConfig.TIM_Period = 40;
	timConfig.TIM_Prescaler = 2;

	TIM_TimeBaseInit(TIM3,&timConfig);

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//enable the Timer interrupt

	TIM_ARRPreloadConfig(TIM3,ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	//Configure the Interrupt
	nvicConfig.NVIC_IRQChannel = TIM2_IRQn;
  	nvicConfig.NVIC_IRQChannelPreemptionPriority = 0;
  	nvicConfig.NVIC_IRQChannelSubPriority = 2; 
  	nvicConfig.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicConfig);
	//Start the timer
	TIM_Cmd(TIM2,ENABLE);

	//Configure the Interrupt
	nvicConfig.NVIC_IRQChannel = TIM3_IRQn;
  	nvicConfig.NVIC_IRQChannelPreemptionPriority = 0;
  	nvicConfig.NVIC_IRQChannelSubPriority = 0; 
  	nvicConfig.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicConfig);
	//Start the timer
	//TIM_Cmd(TIM3,ENABLE);
}

void initAccGyro(void){
	//Configure the Accelerometer
 	accConfig.Power_Mode = LSM_Acc_Lowpower_NormalMode; 
 	accConfig.ODR = LSM_Acc_ODR_100; //100 Hz
 	accConfig.Axes_Enable = LSM_Acc_XYZEN; //enable all axes   
 	accConfig.FS = LSM_Acc_FS_2; //set resolution to 2g
 	accConfig.Data_Update = LSM_Acc_BDU_Continuos;
 	accConfig.Endianess = LSM_Acc_Little_Endian;
	
	//initialize the I2C module and configure it to talk to the accelerometer
 	LSM303DLH_I2C_Init();
 	//configure the accelerometer on the I2C module as above
 	LSM303DLH_Acc_Config(&accConfig);
	//configure the Gyroscope
	LPRYxxxAL_ADC_Init();
	LPRYxxxAL_DMA_Init();
	LPRYxxxAL_PD_ST_Init();
	LPRYxxxAL_On();
}


//this method filters the data and computes the fused roll/pitch
void update_Orientation(void){
   	//computes the sum of the accelerometer readings in the array
	accelX = (rawAcc[0][0] + rawAcc[1][0] + rawAcc[2][0] + rawAcc[3][0]);
	accelY = (rawAcc[0][1] + rawAcc[1][1] + rawAcc[2][1] + rawAcc[3][1]);
	accelZ = (rawAcc[0][2] + rawAcc[1][2] + rawAcc[2][2] + rawAcc[3][2]);	 
	
	//Compute acceleration magnitude in 2 directions
	accelYZ = sqrt(pow(accelY,2) + pow(accelZ,2));
	accelXY = sqrt(pow(accelX,2) + pow(accelY,2));

	//Compute pitch and roll
	pitch = atan_table(accelX/accelYZ);
	roll = atan_table(accelY/accelZ);	
	if(accelZ < 0){
		roll = roll + 180;
		if (roll > 180) {					 //keep values within -180/+180
			roll = roll -360;
		}
	}

    /***************************************************************
    *In order to better determine the translation of the board from 
    *the sensors inputs(especially since we have both the gyroscope 
    *and acccelerometer) a Kalman filter and a Sensor Fusion algorithm.
    *We build a filter based on an exponential model, were we arbitrarily
    *choose to use factors 0.1 and 0.9 for the accelerometer and 
    *gyroscope respectively. We hold a history of 10 and use an
    *exponential model.
    *More info, can be found here: http://en.wikipedia.org/wiki/Kalman_filter
    ***************************************************************/

	//compute the mean and variance of the pitch and roll obtained from
	//the accelerometer
	mean_pitch = mean_pitch - pitch_stat[stat_count]/10 + pitch/10;
	mean_roll = mean_roll - roll_stat[stat_count]/10 + roll/10;
	pitchsquare = pitchsquare - pitch2_stat[stat_count]/10 + pitch*pitch/10;
	rollsquare = rollsquare - roll2_stat[stat_count]/10 + roll*roll/10;
	
	//update history buffers with latest values
	pitch_stat[stat_count] = pitch;
	pitch2_stat[stat_count] = pitch*pitch;
	roll_stat[stat_count] = roll;
	roll2_stat[stat_count] = roll*roll;
	stat_count = (stat_count + 1) % 10;		

    //Calculating the variance
	var_pitch = pitchsquare - mean_pitch*mean_pitch;
	var_roll = rollsquare - mean_roll*mean_roll;

	//compute the fuse factor for each of the roll and pitch angles
	fuse_factor_r = 0.1+0.9*exp(-(var_roll*var_roll/64));
	fuse_factor_p = 0.1+0.9*exp(-(var_pitch*var_pitch/64));

	//obtain the roll, pitch, and yaw averages from the gyroscope
	gyrosp_roll = (rawGyro[0][0] + rawGyro[1][0] + rawGyro[2][0] + rawGyro[3][0])/4;
	gyrosp_pitch = (rawGyro[0][1] + rawGyro[1][1] + rawGyro[2][1] + rawGyro[3][1])/4;
	gyrosp_yaw = (rawGyro[0][2] + rawGyro[1][2] + rawGyro[2][2] + rawGyro[3][2])/4;

	//compute the actual gyroscope roll as the average between the currently obtained
	//roll and the previous value 
	gyro_roll = roll_fuse + (gyrosp_roll+gyrosp_roll_b)/50.0;
	gyrosp_roll_b = gyrosp_roll;

	gyro_pitch = pitch_fuse + (gyrosp_pitch+gyrosp_pitch_b)/50.0;
	gyrosp_pitch_b = gyrosp_pitch;

	gyro_yaw += (gyrosp_yaw+gyrosp_yaw_b)/50.0;	
	gyrosp_yaw_b = gyrosp_yaw;

	//compute the fused roll and pitch
	roll_fuse = fuse_factor_r*roll + (1-fuse_factor_r)*(roll_fuse+(gyrosp_roll+gyrosp_roll_b)/50.0);
	
	pitch_fuse = fuse_factor_p*pitch + (1-fuse_factor_p)*(pitch_fuse+(gyrosp_pitch+gyrosp_pitch_b)/50.0);
		
		
}

symbol_t latching_move;
symbol_t latched_move;
u8 latching = 0;

//The Timer Interrupt Service Routine
__irq void TIM2_IRQHandler(void){
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //clear the pending Interrupt
	  if (1 == latching) {
		LSM303DLH_Acc_Read_Acc(rawAcc[data]); //update the Accelerometer value
		LPRYxxxAL_Read_Rate(rawGyro[data]);	//update the Gyroscope value
		data = (data + 1) % 4;  //set the counter up
		newData = 1; //indicate that new data is available
	 
	 	latched_move = get_move();

		// if the move matches what we're latching...
		if (latched_move & latching_move) {
			// set latching to 2 and generate an interrupt. the handler for this interrupt is in protocol.c
			latching = 2;
			EXTI_GenerateSWInterrupt(EXTI_Line11);	
		}
	}
}

// start latching move. the check is done in the tim2 interrupt
void latch_move (symbol_t lmove) {
	  latching_move = lmove;
	  latching = 1;
}

symbol_t get_move (void) { 
	symbol_t tmp;

	// if there is new data, reset newData to 0 and return the move
	if(newData){
		 update_Orientation();
	
		 if (updateGesture(accelX, accelY, accelZ, roll, &move) == SUCCESS);					 

		 newData = 0;
		 return move;
	 }
	 // otherwise, if latching is 2, return the latest recorded move. this is designed to return a move only once
	 else if (2 == latching) {
	 	latching = 0;
		tmp = move;
		move = no_move;
		return tmp;
	 }

	 // otherwise, return no_move
	 return no_move;
	 
}
