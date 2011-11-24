/**
  * @file    iNEMO_lib.c
  * @author  ART Team IMS-Systems Lab
  * @version V2.0.0
  * @date    09/20/2010
  * @brief   Sensor Hardware Configuration & Setup & API.
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



/* Includes */
#include "iNEMO_lib.h"
#include "iNEMO_conf.h"
#include "utils.h"

/**
 * @addtogroup iNEMO_Config
 * @{
 */


/* Private define */
#define BlockSize            512 /*!< Block Size in Bytes */
#define BufferWordsSize      (BlockSize >> 2)
#define NumberOfBlocks       2  /*!< For Multi Blocks operation (Read/Write) */
#define MultiBufferWordsSize ((BlockSize * NumberOfBlocks) >> 2)

#define iNEMO_OffAccX(data)    (data->sOffset[0])
#define iNEMO_OffAccY(data)    (data->sOffset[1])
#define iNEMO_OffAccZ(data)    (data->sOffset[2])

#define iNEMO_OffGyroX(data)    (data->sOffset[3])
#define iNEMO_OffGyroY(data)    (data->sOffset[4])
#define iNEMO_OffGyroZ(data)    (data->sOffset[5])

#define iNEMO_OffMagnX(data)    (data->sOffset[6])
#define iNEMO_OffMagnY(data)    (data->sOffset[7])
#define iNEMO_OffMagnZ(data)    (data->sOffset[8])

#define iNEMO_OffPress(data)    (data->sOffset[9])
#define iNEMO_OffTemp(data)     (data->sOffset[10])


#define iNEMO_GainAccX(data)     (data->uGain[0])
#define iNEMO_GainAccY(data)     (data->uGain[1])
#define iNEMO_GainAccZ(data)     (data->uGain[2])

#define iNEMO_GainGyroX(data)    (data->uGain[3])
#define iNEMO_GainGyroY(data)    (data->uGain[4])
#define iNEMO_GainGyroZ(data)    (data->uGain[5])

#define iNEMO_GainMagnX(data)    (data->uGain[6])
#define iNEMO_GainMagnY(data)    (data->uGain[7])
#define iNEMO_GainMagnZ(data)    (data->uGain[8])

#define iNEMO_GainPress(data)    (data->uGain[9])
#define iNEMO_GainTemp(data)     (data->uGain[10])




/* Private variables */
volatile TestStatus EraseStatus = FAILED, TransferStatus1 = FAILED, TransferStatus2 = FAILED;
static u32 s_Buffer_Block_Tx[BufferWordsSize];
static u32 s_Buffer_Block_Rx[BufferWordsSize];
static u32 s_Buffer_MultiBlock_Tx[MultiBufferWordsSize];
static u32 s_Buffer_MultiBlock_Rx[MultiBufferWordsSize];



/**
 * @addtogroup iNEMO_Config_Function
 *@{
 */


/**
* @brief Configure the hardware for the iNEMO platform
* */
void iNEMO_HW_Config(void)
{

#ifdef _ULED
  iNEMO_Led_Init(LED1);
#endif //_ULED

#ifdef _VCOM
  iNEMO_USB_Disconnet_Pin_Config();
  USB_Cable_Config(ENABLE);  
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
#endif //_VCOM


#ifdef _PRESS
  LPS001DL_I2C_Init();
  #ifdef INT1_PAD_ENABLE
  LPS001DL_INT1_Config();
  #endif //INT1_PAD_ENABLE
#endif //_PRESS

#ifdef _TEMP
  STLM75_I2C_Init();
#endif //_TEMP

#ifdef _SDIO
  SD_Init();
#endif // _SDIO

#ifdef _PBUTT
  iNEMO_Button_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
#endif //_PBUTT

#ifdef _6X
  LSM303DLH_I2C_Init();
#endif

#ifdef _GYRO_RPY
  LPRYxxxAL_ADC_Init();
  LPRYxxxAL_DMA_Init();
  LPRYxxxAL_PD_ST_Init();
#endif
}

/**
* @brief  Initialize the iNEMO data structur
*/
void iNEMO_Data_Init(iNEMO_DATA* data)
{
  iNEMO_OffAccX(data)=iNEMO_Default_OffAccX;
  iNEMO_OffAccY(data)=iNEMO_Default_OffAccY;
  iNEMO_OffAccZ(data)=iNEMO_Default_OffAccZ;

  iNEMO_OffGyroX(data)=iNEMO_Default_OffGyroX;
  iNEMO_OffGyroY(data)=iNEMO_Default_OffGyroY;
  iNEMO_OffGyroZ(data)=iNEMO_Default_OffGyroZ;

  iNEMO_OffMagnX(data)=iNEMO_Default_OffMagnX;
  iNEMO_OffMagnY(data)=iNEMO_Default_OffMagnY;
  iNEMO_OffMagnZ(data)=iNEMO_Default_OffMagnZ;

  iNEMO_OffPress(data)=iNEMO_Default_OffPress;
  iNEMO_OffTemp(data)=iNEMO_Default_OffTemp;

  iNEMO_GainAccX(data)= iNEMO_Default_GainAccX;
  iNEMO_GainAccY(data)= iNEMO_Default_GainAccY;
  iNEMO_GainAccZ(data)= iNEMO_Default_GainAccZ;

  iNEMO_GainGyroX(data)=iNEMO_Default_GainGyroX;
  iNEMO_GainGyroY(data)=iNEMO_Default_GainGyroY;
  iNEMO_GainGyroZ(data)=iNEMO_Default_GainGyroZ;

  iNEMO_GainMagnX(data)=iNEMO_Default_GainMagnX;
  iNEMO_GainMagnY(data)=iNEMO_Default_GainMagnY;
  iNEMO_GainMagnZ(data)=iNEMO_Default_GainMagnZ;

  iNEMO_GainPress(data)=iNEMO_Default_GainPress;
  iNEMO_GainTemp(data)=iNEMO_Default_GainTemp;
  
}


/**
* @brief  Initialize the iNEMO sensor platform
*/
void iNEMO_Config(void)
{

#ifdef _GYRO_RPY
 LPRYxxxAL_On();
 LPRYxxxAL_ST_Off();
#endif

#ifdef _6X

  LSM_Acc_ConfigTypeDef  LSM_Acc_InitStructure;
  LSM_Acc_Filter_ConfigTypeDef LSM_Acc_FilterStructure;
  LSM_Acc_InitStructure.Power_Mode = LSM_Acc_Lowpower_NormalMode;
  LSM_Acc_InitStructure.ODR = LSM_Acc_ODR_50;
  LSM_Acc_InitStructure.Axes_Enable= LSM_Acc_XYZEN;
  LSM_Acc_InitStructure.FS = LSM_Acc_FS_2;
  LSM_Acc_InitStructure.Data_Update = LSM_Acc_BDU_Continuos;
  LSM_Acc_InitStructure.Endianess=LSM_Acc_Big_Endian;

  LSM_Acc_FilterStructure.HPF_Enable=LSM_Acc_Filter_Disable;
  LSM_Acc_FilterStructure.HPF_Mode=LSM_Acc_FilterMode_Normal;
  LSM_Acc_FilterStructure.HPF_Reference=0x00;
  LSM_Acc_FilterStructure.HPF_Frequency=LSM_Acc_Filter_HPc8;

  LSM303DLH_Acc_Config(&LSM_Acc_InitStructure);
  LSM303DLH_Acc_Filter_Config(&LSM_Acc_FilterStructure);

  LSM_Magn_ConfigTypeDef LSM_Magn_InitStructure;
  LSM_Magn_InitStructure.M_ODR = LSM_Magn_ODR_30;
  LSM_Magn_InitStructure.Meas_Conf = LSM_Magn_MEASCONF_NORMAL;
  LSM_Magn_InitStructure.Gain = LSM_Magn_GAIN_1_3;
  LSM_Magn_InitStructure.Mode = LSM_Magn_MODE_CONTINUOS ;
  LSM303DLH_Magn_Config(&LSM_Magn_InitStructure);

#endif

#ifdef _ACC
  LIS331DLH_ConfigTypeDef  LIS331DLH_InitStructure;
  LIS331DLH_Filter_ConfigTypeDef LIS331DLH_FilterStructure;
  LIS331DLH_InitStructure.Power_Mode = LIS331DLH_Lowpower_NormalMode;
  LIS331DLH_InitStructure.ODR = LIS331DLH_ODR_50;
  LIS331DLH_InitStructure.Axes_Enable= LIS331DLH_XYZEN;
  LIS331DLH_InitStructure.FS = LIS331DLH_FS_2;
  LIS331DLH_InitStructure.Data_Update = LIS331DLH_BDU_Continuos;
  LIS331DLH_InitStructure.Endianess=LIS331DLH_Little_Endian;

  LIS331DLH_FilterStructure.HPF_Enable=LIS331DLH_Filter_Disable;
  LIS331DLH_FilterStructure.HPF_Mode=LIS331DLH_FilterMode_Normal;
  LIS331DLH_FilterStructure.HPF_Reference=0x00;
  LIS331DLH_FilterStructure.HPF_Frequency=LIS331DLH_Filter_HPc64;

  LIS331DLH_Config(&LIS331DLH_InitStructure);
  LIS331DLH_Filter_Config(&LIS331DLH_FilterStructure);
#endif /*_ACC */

#ifdef _MAG
  HMC5843_ConfigTypeDef HMC5843_InitStructure;
  HMC5843_InitStructure.M_ODR = HMC5843_ODR_10;
  HMC5843_InitStructure.Meas_Conf = HMC5843_MEASCONF_NORMAL;
  HMC5843_InitStructure.Gain = HMC5843_GAIN_1;
  HMC5843_InitStructure.Mode = HMC5843_MODE_CONTINUOS ;
  HMC5843_Config(&HMC5843_InitStructure);
#endif /*_MAG */

#ifdef _PRESS
  LPS001DL_ConfigTypeDef  LPS001DL_InitStructure;
  LPS001DL_InterruptTypeDef LPS001DL_InterruptStructure;
  /* LPS001DL configuration */
  LPS001DL_InitStructure.Lowpower = LPS001DL_Lowpower_Disable;
  LPS001DL_InitStructure.Output_Data_Rate = LPS001DL_Output_Data_Rate_12_12;
  LPS001DL_InitStructure.Data_Update = LPS001DL_BDU_Continuos; //LPS001DL_BDU_Single;
  LPS001DL_InitStructure.Endianess = LPS001DL_Little_Endian;

  /* LPS001DL interrupt configuration */
  LPS001DL_InterruptStructure.Mode = LPS001DL_Interrupt_Active_High;
  LPS001DL_InterruptStructure.PP_OD = LPS001DL_Interrupt_Push_Pull;
  LPS001DL_InterruptStructure.Int2_Pad_Mode =LPS001DL_Interrupt_Pressure_Low;
  LPS001DL_InterruptStructure.Int1_Pad_Mode = LPS001DL_Interrupt_Pressure_High;
  LPS001DL_InterruptStructure.Latch = LPS001DL_Interrupt_Latch_Disable;
  LPS001DL_InterruptStructure.Delta_Press_Low = LSP001DL_Interrupt_DPL_Enable;
  LPS001DL_InterruptStructure.Delta_Press_High = LSP001DL_Interrupt_DPH_Enable;

  LPS001DL_Config(&LPS001DL_InitStructure);
  LPS001DL_Interrupt_Config(&LPS001DL_InterruptStructure);
  LPS001DL_PowerDown_Cmd(DISABLE);
#endif /*_PRESS */

#ifdef _TEMP
#ifdef OS_INT_ENABLE
  STLM75_InterruptTypeDef STLM75_InterruptStructure;
  /* STLM75 interrupt configuration */
  STLM75_InterruptStructure.Mode = STLM75_Interrupt_Mode; //STLM75_Comparator_Mode;
  STLM75_InterruptStructure.Polarity = STLM75_Polarity_Low;
  STLM75_InterruptStructure.Fault_Number = Fault_Value_1;
  STLM75_InterruptStructure.Over_Limit_Value = 305;
  STLM75_InterruptStructure.Hysteresis_Value = 290;

  STLM75_Interrupt_Config(&STLM75_InterruptStructure);

#endif //OS_INT_ENABLE
#endif /*_TEMP */

}


/**
* @brief Test SDIO
*/
#ifdef _SDIO

TestStatus Test_SDIO(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    SD_CardInfo SDCardInfo;
    SD_Error Status = SD_OK;

	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 11;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	/* SD Init   */
	Status = SD_Init();

	if (Status == SD_OK)
	{
		/*  Read CSD/CID MSD registers  */
		Status = SD_GetCardInfo(&SDCardInfo);
	}

	if (Status == SD_OK)
	{
		/* Select Card  -*/
		Status = SD_SelectDeselect((u32) (SDCardInfo.RCA << 16));
	}

	if (Status == SD_OK)
	{
		Status = SD_EnableWideBusOperation(SDIO_BusWide_4b);
	}

	/*  Block Erase  */
	if (Status == SD_OK)
	{
		/* Erase NumberOfBlocks Blocks of WRITE_BL_LEN(512 Bytes) */
		Status = SD_Erase(0x00, (BlockSize * NumberOfBlocks));
	}

	/* Set Device Transfer Mode to DMA */
	if (Status == SD_OK)
	{
		Status = SD_SetDeviceMode(SD_DMA_MODE);
	}

	if (Status == SD_OK)
	{
		Status = SD_ReadMultiBlocks(0x00, s_Buffer_MultiBlock_Rx, BlockSize, NumberOfBlocks);
	}

	if (Status == SD_OK)
	{
		EraseStatus = eBuffercmp(s_Buffer_MultiBlock_Rx, MultiBufferWordsSize);
	}

	/*  Block Read/Write  */
	/* Fill the buffer to send */
	Fill_Buffer(s_Buffer_Block_Tx, BufferWordsSize, 0xFFFF);


	if (Status == SD_OK)
	{
		/* Write block of 512 bytes on address 0 */
		Status = SD_WriteBlock(0x00, s_Buffer_Block_Tx, BlockSize);
	}

	if (Status == SD_OK)
	{
		/* Read block of 512 bytes from address 0 */
		Status = SD_ReadBlock(0x00, s_Buffer_Block_Rx, BlockSize);
	}

	if (Status == SD_OK)
	{
		/* Check the corectness of written dada */
		TransferStatus1 = Buffercmp(s_Buffer_Block_Tx, s_Buffer_Block_Rx, BufferWordsSize);
	}

	/*  Multiple Block Read/Write */
	/* Fill the buffer to send */
	Fill_Buffer(s_Buffer_MultiBlock_Tx, MultiBufferWordsSize, 0x0);

	if (Status == SD_OK)
	{
		/* Write multiple block of many bytes on address 0 */
		Status = SD_WriteMultiBlocks(0x00, s_Buffer_MultiBlock_Tx, BlockSize, NumberOfBlocks);
	}

	if (Status == SD_OK)
	{
		/* Read block of many bytes from address 0 */
		Status = SD_ReadMultiBlocks(0x00, s_Buffer_MultiBlock_Rx, BlockSize, NumberOfBlocks);
	}

	if (Status == SD_OK)
	{
		/* Check the corectness of written dada */
		TransferStatus2 = Buffercmp(s_Buffer_MultiBlock_Tx, s_Buffer_MultiBlock_Rx, MultiBufferWordsSize);
	}
	if(EraseStatus==PASSED && TransferStatus1==PASSED && TransferStatus2==PASSED)
		return PASSED;
	else
		return FAILED;

}

#else

/* If the _SDIO macro is not defined in iNEMO_conf.h file, the test simply return FAILED*/
TestStatus Test_SDIO(void) {
	return FAILED;
}
#endif

/**
 * @}
 */  /* end of group iNEMO_Config_Function */

/**
 * @}
 */  /* end of group iNEMO_Config */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
