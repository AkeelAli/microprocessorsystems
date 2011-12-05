#include "gesture_latch.h"


/* global variables	 */
LSM_Acc_ConfigTypeDef accConfig;
NVIC_InitTypeDef nvicConfig;
TIM_TimeBaseInitTypeDef timConfig;

/*	arrays for raw Accelerometer and Gyroscope 
	need 4 slots in each of the 3D buffers for 
	filtering */ 
s16 rawAcc[4][3];

//set when there is new raw data after a TIM2 interrupt
short newData = 0;	 
//counter for raw data placement in buffer (counts up to 3 then wraps back to 0)
int data = 0; 

int32_t accelX,accelY,accelZ;
float accelYZ,accelXY;
float roll, pitch;

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
  	//sums all 4 previous readings for filtering
	accelX = (rawAcc[0][0] + rawAcc[1][0] + rawAcc[2][0] + rawAcc[3][0]);
	accelY = (rawAcc[0][1] + rawAcc[1][1] + rawAcc[2][1] + rawAcc[3][1]);
	accelZ = (rawAcc[0][2] + rawAcc[1][2] + rawAcc[2][2] + rawAcc[3][2]);	 
	
	//Compute acceleration magnitude in 2 directions
	accelYZ = sqrt(1.0*pow(accelY,2) + pow(accelZ,2));
	accelXY = sqrt(1.0*pow(accelX,2) + pow(accelY,2));

	//Compute pitch and roll
	pitch = atan_table(1.0*accelX/accelYZ);
	roll = atan_table(1.0*accelY/accelZ);	
	if(accelZ < 0){
		roll = roll + 180;
		if (roll > 180) {	//keep values within -180/+180
			roll = roll - 360;
		}
	}		
		
}

symbol_t latching_move;
symbol_t latched_move;
u8 latching = 0;

//The Timer Interrupt Service Routine
__irq void TIM2_IRQHandler(void){
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //clear the pending Interrupt
	  if (1 == latching) {
		LSM303DLH_Acc_Read_Acc(rawAcc[data]); //update the Accelerometer value
		//LPRYxxxAL_Read_Rate(rawGyro[data]);	//update the Gyroscope value
		data = (data + 1) % 4;  //set the counter up
		newData = 1; //indicate that new data is available
	 
	 	latched_move = get_move();
		if (latched_move & latching_move) {
			latching = 2;
			EXTI_GenerateSWInterrupt(EXTI_Line11);	
		}
	}
}




void latch_move (symbol_t lmove) {
	  latching_move = lmove;
	  latching = 1;
}

symbol_t get_move (void) { 
	symbol_t tmp;
	if(newData){
		 update_Orientation();
	
		 if (updateGesture(accelX, accelY, accelZ, roll, &move) == SUCCESS);					 

		 newData = 0;
		 return move;
	 }
	 else if (2 == latching) {
	 	latching = 0;
		tmp = move;
		move = no_move;
		return tmp;
	 }
	 return no_move;
	 
}
 
