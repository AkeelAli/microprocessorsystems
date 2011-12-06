//include library header files
#include <stdio.h>
#include "stm32f10x_conf.h"
#include "LSM303DLH.h"
#include "LPRYxxxAL.h"
#include "atan_LUT.h"
#include <math.h>
#include "gestures.h"

#include "spi_protocol.h"
#include "cc2500.h"

//Define constants/Macros
#define PI 3.141592654

void initTIM(void);
void initAccGyro(void);
void update_Orientation(void);
//global variables
LSM_Acc_ConfigTypeDef accConfig;
NVIC_InitTypeDef nvicConfig;
TIM_TimeBaseInitTypeDef timConfig;
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

symbol_t move; //UpdateGesture stores the move in this variable

uint16_t readout=0;

int main(void){
	
	//initialize Accelerometer	and Gyroscope
	initAccGyro();
	//initialize the Timer Interrupt
	initTIM();

							SPI_Configuration();
							RF_Config();
						
	while(1){
	  if(newData){
		 update_Orientation();
	
		 if (updateGesture(accelX, accelY, accelZ, roll_fuse, &move) == SUCCESS);
		 
							SPI_Write(0x2C, 0xFF);
							readout= SPI_Read(0x2C);

		 newData = 0;
	  }
	
	}  


	
/*	while(1)
	RF_Receive(&readout);
  */
	/*while(1) {
		SPI_Write(0x2C, 0xFF);
		readout= SPI_Read(0x2C);
		
	}  */

//	SPI_Write(0x2C, 0xFF);
//	readout = SPI_Read(0x2C);

}

void initTIM(void){
	//ENABLE CLOCK FOR TIM 2's CLOCK
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	//Set up the timer
	timConfig.TIM_Period = 80;
	timConfig.TIM_Prescaler = 999; //Want to trigger every 10ms 8Meg/(80*(999+1)) = 100 (100Hz)
	timConfig.TIM_CounterMode = TIM_CounterMode_Up;
  	timConfig.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM2,&timConfig);

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//enable the Timer interrupt

	TIM_ARRPreloadConfig(TIM2,ENABLE);

	//Configure the Interrupt
	nvicConfig.NVIC_IRQChannel = TIM2_IRQn;
  	nvicConfig.NVIC_IRQChannelPreemptionPriority = 2;
  	nvicConfig.NVIC_IRQChannelSubPriority = 2; 
  	nvicConfig.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicConfig);
	//Start the timer
	TIM_Cmd(TIM2,ENABLE);
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
 //The Timer Interrupt Service Routine
__irq void TIM2_IRQHandler(void){
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //clear the pending Interrupt
	LSM303DLH_Acc_Read_Acc(rawAcc[data]); //update the Accelerometer value
	LPRYxxxAL_Read_Rate(rawGyro[data]);	//update the Gyroscope value
	data = (data + 1) % 4;  //set the counter up
	newData = 1; //indicate that new data is available
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

	var_pitch = pitchsquare - mean_pitch*mean_pitch;
	var_roll = rollsquare - mean_roll*mean_roll;

	//compute the fuse factor for each of the roll and pitch angles
	fuse_factor_r = 0.1+0.9*exp(-(var_roll*var_roll/64));
	fuse_factor_p = 0.1+0.9*exp(-(var_pitch*var_pitch/64));
	//obtain roll, pitch, and yaw speed from the gyroscope
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


