/**
  * @file    iNEMO_lib.h
  * @author  ART Team IMS-Systems Lab
  * @version V2.0.0
  * @date    09/20/2010
  * @brief   This file includes the sensor and features header files in the user application.
  * @details
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
  * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
  * IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */



/* Define to prevent recursive inclusion */
#ifndef __iNEMO_LIB_H
#define __iNEMO_LIB_H

/* includes*/
#include "stm32f10x.h"
#include "iNEMO_conf.h"
#include "utils.h"

/**
* @addtogroup iNEMO_Config
* @{
*/


/**
* @defgroup  Gain_and_Offset_Default_Values
* @brief Default value coming from datasheet
* @{
*/
#define iNEMO_Default_OffAccX    0 
#define iNEMO_Default_OffAccY    0 
#define iNEMO_Default_OffAccZ    0 

#define iNEMO_Default_OffGyroX   0
#define iNEMO_Default_OffGyroY   0
#define iNEMO_Default_OffGyroZ   0

#define iNEMO_Default_OffMagnX   0
#define iNEMO_Default_OffMagnY   0
#define iNEMO_Default_OffMagnZ   0

#define iNEMO_Default_OffPress   0
#define iNEMO_Default_OffTemp    0


#define iNEMO_Default_GainAccX     LSM_Acc_Sensitivity_2g
#define iNEMO_Default_GainAccY     LSM_Acc_Sensitivity_2g
#define iNEMO_Default_GainAccZ     LSM_Acc_Sensitivity_2g

#define iNEMO_Default_GainGyroX    Gyro_SensitivityLSB_PR
#define iNEMO_Default_GainGyroY    Gyro_SensitivityLSB_PR
#define iNEMO_Default_GainGyroZ    Gyro_SensitivityLSB_Yaw

#define iNEMO_Default_GainMagnX    LSM_Magn_Sensitivity_XY_1_3Ga
#define iNEMO_Default_GainMagnY    LSM_Magn_Sensitivity_XY_1_3Ga
#define iNEMO_Default_GainMagnZ    LSM_Magn_Sensitivity_Z_1_3Ga

#define iNEMO_Default_GainPress    P_Conversion
#define iNEMO_Default_GainTemp     T_Resolution

/**
* @}
*/ /* end of group Gain_and_Offset_Default_Values */ 

#ifdef AHRS_MOD
#include "iNEMO_AHRS.h"
#endif

#ifdef _6X
  #include "HAL_LSM303DLH.h"
  #include "LSM303DLH.h"
#endif //_6X

#ifdef _GYRO_RPY
  #include "HAL_LPRYxxxAL.h"
  #include "LPRYxxxAL.h"
#endif

#ifdef _ACC
  #include "LIS331DLH_SPI.h"
  #include "HAL_LIS331DLH.h"
#endif //_ACC

#ifdef _PRESS
  #include "HAL_LPS001DL.h"
  #include "LPS001DL_I2C.h"
#endif // _PRESS

#ifdef _TEMP
  #include "HAL_STLM75.h"
  #include "STLM75.h"
#endif //_TEMP

#ifdef _VCOM
  #include "virtual_com.h"
#endif /*_VCOM */

#ifdef _SDIO
  #include "sdcard.h"
#endif /*_SDIO */

#ifdef _ULED
  #include "iNEMO_led.h"
#endif /*_ULED */

#ifdef _PBUTT
  #include "iNEMO_button.h"
#endif /*_PBUTT */



/**
 * @struct
 * @brief sensor data struct
 */

typedef struct
{

  s16 sAcc[3];
  s16 sGyro[3];
  s16 sMag[3];
  
  s16 sTemp;
  u16 uPress;

  float uGain[11]; /*AccX AccY AccZ - GyroX GyroY GyroZ - MagnX MagnY MagnZ - Press - Temp*/
  s16 sOffset[11];      /*AccX AccY AccZ - GyroX GyroY GyroZ - MagnX MagnY MagnZ - Press - Temp*/
  
#ifdef AHRS_MOD
  iNEMO_QUAT          m_quat;
  iNEMO_SENSORDATA    m_sensorData;
  iNEMO_EULER_ANGLES  m_angle;
#endif   
} iNEMO_DATA;



void iNEMO_HW_Config(void);
void iNEMO_Config(void);
void iNEMO_Data_Init(iNEMO_DATA* data);
TestStatus Test_SDIO(void);

/**
 * @}
 */ /* end of group iNEMO_Config */ 

#endif /* __iNEMO_LIB_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
