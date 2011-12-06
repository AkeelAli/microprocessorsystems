#include "rf2500.h"


u16 t,t2;
u8 t3,tmp;
u8 _latest_byte = 0;
u8 _new_rf_data = 0;

u8 *timer_lock;
u16 _milliseconds = 0;
u16 _wait_time = 0;


void init_spi (void) {

	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(
		RCC_APB2Periph_GPIOA | 
		RCC_APB2Periph_AFIO |
		RCC_APB2Periph_SPI1, 
		ENABLE
	);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);



	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_SSOutputCmd(SPI1, ENABLE);
	
	SPI_Cmd(SPI1, ENABLE);




	/* Connect EXTI Line6 to PA6 - MISO I pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource6);


	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);




	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);	

	
	raise_css();

	rf_reset();
	
	configure();

	//send_strobe	 (TI_CCxxx0_SFSTXON);
	//send_strobe	 (TI_CCxxx0_SRX);
	//rf_send_byte (0x13);
	//test_send();
//	SPI_I2S_SendData(SPI1, TI_CCxxx0_SRES);


}

// just raise/lower css and read back until the desired value is on
void raise_css (void) {
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)1);
	while (!GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_4));		  // wait until bit actually changes

}
void drop_css(void){
  	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)0);
	while (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_4));	  // wait until bit actually changes
}

// will return the status bits (except for the MSB)
RFStatus rf_get_status(void) {
 	return (RFStatus) (0x70 & send_strobe(TI_CCxxx0_SNOP));
}
// will return the free bytes (for writing)
u8 rf_get_free_bytes(void) {
 	return	0x0F & send_strobe(TI_CCxxx0_SNOP);
}

// reset the chip
void rf_reset (void) {
	u8 l = 1;
 	
	drop_css();
	raise_css();
	
	// wait (at least) 40 mircoseconds
	wait(5,&l);
	while (l);

   	// then send the reset strobe
	send_strobe	 (TI_CCxxx0_SRES);

//	t2=2000;
//	while(t2--)
//	t = SPI1->DR;
	
	
}



// this works!	 don't touch anything
u16 write_byte (u16 address, u16 byte) {

	 
	// drop the css and wait until chip is ready
	drop_css();
	while(miso_high());

	// write the write command
	SPI1->DR = address << 8 | byte;
	
	// wait until no longer busy
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);

	// wait until data in the read register
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

	// read the data; this is the status
	t = SPI1->DR;

	raise_css();
	
	return t;

}

// send an 8 bit strobe command
u16 send_strobe (u8 strobe) {
 	// drop the css and wait until chip is ready
	drop_css();
	while(miso_high());

	// switch mode to 8 bit
	SPI_DataSizeConfig(SPI1,SPI_DataSize_8b);

	// send the strobe
	SPI1->DR = strobe;

	// wait until no longer busy
	while (SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) == SET);

	// wait until there is data in the read register
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

	// read the data
	t = SPI1->DR;

	// switch to 16 bit and release spi
	SPI_DataSizeConfig(SPI1,SPI_DataSize_16b);
	raise_css();

	return t;
}
// this works!	 don't touch anything
u16 read_byte (u16 address) {
	
	// drop the css and wait until chip is ready
	drop_css();
	while(miso_high());

	// send the read command
	SPI1->DR = (address | TI_CCxxx0_READ_SINGLE)<<8;

	// wait until there is data in the read register
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

	// read the data
	t = SPI1->DR;

	// wait until chip is no longer busy before releasing/raising css
	while (SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) == SET);
	raise_css();   

	return t;

}

 RFStatus status;

 RFStatus statuses[128];

// sends a byte using the rf chip
void rf_send_byte (u8 byte) {
   
    switch ( rf_get_status () ) {
        
		// perform various operations depending on state, most notably flush    
        case RF_STATUS_RXFIFO_OVERFLOW:
            send_strobe     (TI_CCxxx0_SFRX);

        case RF_STATUS_TXFIFO_UNDERFLOW:
            send_strobe     (TI_CCxxx0_SFTX);

        case RF_STATUS_IDLE:
           	send_strobe     (TI_CCxxx0_SFTX);
            send_strobe     (TI_CCxxx0_SFSTXON);
            while (rf_get_status() != RF_STATUS_FSTXON);
       
        default:        
                 
             tmp = 0;
             
             // send 33 bytes to the fifo
             while (tmp < 33) {
                  t3 = write_byte(TI_CCxxx0_TXFIFO,byte);
                  tmp++;
             }

			 // send the packet. one packet is 32 bytes
             send_strobe (TI_CCxxx0_STX); 
             while (rf_get_status() == RF_STATUS_TX);
             

			 // flush the buffer
             send_strobe     (TI_CCxxx0_SFTX);             
    }

}

// deprecated variables...
u8 read_lock = 0;

u8 data_rf[32];

// read a byte using the rf chip
u8 rf_read_byte (u16 waittime) {
 	
	switch ( rf_get_status () ) {
	 	// perform various operations depending on state, most notably flush    	
		case RF_STATUS_RXFIFO_OVERFLOW:
			send_strobe	 (TI_CCxxx0_SFRX);

		case RF_STATUS_TXFIFO_UNDERFLOW:
			send_strobe	 (TI_CCxxx0_SFTX);

		
		default:
			send_strobe	 (TI_CCxxx0_SFRX); 		
			_new_rf_data = 0;
			
			// switch to receive state	 
			send_strobe	 (TI_CCxxx0_SRX);
			while (rf_get_status() != RF_STATUS_RX);
			
			// wait the timeout time or until there is new data.
			wait (waittime,&read_lock);
			while (read_lock && !_new_rf_data);

			// if new data, read it from the buffer, flush the buffer and return the byte
			if (_new_rf_data) {
				_new_rf_data = 0;
				tmp = 0;
				while (tmp < 32)
					data_rf[tmp++] = read_byte(TI_CCxxx0_RXFIFO);

				send_strobe	 (TI_CCxxx0_SFRX);

				// the byte returned is the mode of 32 bytes. they should all be the same, but they not always are.
				return findMode(data_rf);	 
			}

			// otherwise go to idle and return 0
			send_strobe	 (TI_CCxxx0_SIDLE);
			return 0x00;
			 
	}

}
void test_send(void) {
	t3 = rf_read_byte(40000);
	t3 = rf_read_byte(40000);
	t3 = rf_read_byte(40000);
	t3 = rf_read_byte(40000);	
	  

}
FlagStatus s;

// return 1 if MISO is high
uint8_t miso_high (void) {

	return GPIO_ReadInputDataBit (GPIOA,GPIO_Pin_6);
	
}




// write a byte to a register, then read it back.
u8 set_config (u16 address, u16 byte) {
 	 write_byte (address,byte);	  
	 
	 if (byte != read_byte(address))
	 	return 1;
	 else
		return 0;
}

// write all the configuration values
u8 configure (void) {
		
	   while (set_config(TI_CCxxx0_IOCFG0,		SETTING_IOCFG0				)) ;
	   while (set_config(TI_CCxxx0_IOCFG1,		SETTING_IOCFG1				)) ;
	   while (set_config(TI_CCxxx0_IOCFG2,		SMARTRF_SETTING_IOCFG2		)) ;


	   while (set_config(TI_CCxxx0_MCSM0,		SETTING_MCSM0				)) ;
	   while (set_config(TI_CCxxx0_PKTCTRL0,	SETTING_PKTCTRL0			)) ;	 
 	   while (set_config(TI_CCxxx0_FSCTRL1,		SMARTRF_SETTING_FSCTRL1		)) ;
	   while (set_config(TI_CCxxx0_FSCTRL0,		SMARTRF_SETTING_FSCTRL0		)) ;
	   while (set_config(TI_CCxxx0_FREQ2,		SMARTRF_SETTING_FREQ2		)) ;
	   while (set_config(TI_CCxxx0_FREQ1,		SMARTRF_SETTING_FREQ1		)) ;
	   while (set_config(TI_CCxxx0_FREQ0,		SMARTRF_SETTING_FREQ0		)) ;
	   while (set_config(TI_CCxxx0_MDMCFG4,		SMARTRF_SETTING_MDMCFG4		)) ;
	   while (set_config(TI_CCxxx0_MDMCFG3,		SMARTRF_SETTING_MDMCFG3		)) ;
	   while (set_config(TI_CCxxx0_MDMCFG2,		SMARTRF_SETTING_MDMCFG2		)) ;
	   while (set_config(TI_CCxxx0_MDMCFG1,		SMARTRF_SETTING_MDMCFG1		)) ;
	   while (set_config(TI_CCxxx0_MDMCFG0,		SMARTRF_SETTING_MDMCFG0		)) ;
	   while (set_config(TI_CCxxx0_CHANNR,		0x21		)) ;
	   while (set_config(TI_CCxxx0_DEVIATN,		SMARTRF_SETTING_DEVIATN		)) ;
	   while (set_config(TI_CCxxx0_FREND1,		SMARTRF_SETTING_FREND1		)) ;
	   while (set_config(TI_CCxxx0_FREND0,		SMARTRF_SETTING_FREND0		)) ;
	   while (set_config(TI_CCxxx0_MCSM0,		SMARTRF_SETTING_MCSM0		)) ;
	   while (set_config(TI_CCxxx0_MCSM1,		SETTING_MCSM1				)) ;


	   while (set_config(TI_CCxxx0_FOCCFG,		SMARTRF_SETTING_FOCCFG		)) ;
	   while (set_config(TI_CCxxx0_BSCFG,		SMARTRF_SETTING_BSCFG		)) ;
	   while (set_config(TI_CCxxx0_AGCCTRL2,	SMARTRF_SETTING_AGCCTRL2	)) ;
	   while (set_config(TI_CCxxx0_AGCCTRL1,	SMARTRF_SETTING_AGCCTRL1	)) ;
	   while (set_config(TI_CCxxx0_AGCCTRL0,	SMARTRF_SETTING_AGCCTRL0	)) ;
	   while (set_config(TI_CCxxx0_FSCAL3,		SMARTRF_SETTING_FSCAL3		)) ;
	   while (set_config(TI_CCxxx0_FSCAL2,		SMARTRF_SETTING_FSCAL2		)) ;
	   while (set_config(TI_CCxxx0_FSCAL1,		SMARTRF_SETTING_FSCAL1		)) ;
	   while (set_config(TI_CCxxx0_FSCAL0,		SMARTRF_SETTING_FSCAL0		)) ;
	   while (set_config(TI_CCxxx0_FSTEST,		SMARTRF_SETTING_FSTEST		)) ;
	   while (set_config(TI_CCxxx0_TEST2,		SMARTRF_SETTING_TEST2		)) ;
	   while (set_config(TI_CCxxx0_TEST1,		SMARTRF_SETTING_TEST1		)) ;
	   while (set_config(TI_CCxxx0_TEST0,		SMARTRF_SETTING_TEST0		)) ;
	   while (set_config(TI_CCxxx0_FIFOTHR,		SMARTRF_SETTING_FIFOTHR		)) ;
	   
	   while (set_config(TI_CCxxx0_PKTCTRL1,	SMARTRF_SETTING_PKTCTRL1	)) ;
	   while (set_config(TI_CCxxx0_PKTCTRL0,	SMARTRF_SETTING_PKTCTRL0	)) ;
	   while (set_config(TI_CCxxx0_ADDR,		SMARTRF_SETTING_ADDR		)) ;
	   while (set_config(TI_CCxxx0_PKTLEN,		SMARTRF_SETTING_PKTLEN		)) ;

	   t3 = read_byte(TI_CCxxx0_CHANNR);
	   return 0;

}

// called every time line 6 is raised
__irq void EXTI9_5_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line6) != RESET) {
		// however, valid only if css is high (if it's low, normal spi operation)
		if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_4)) {
			// if high, that means we've received a new packet		
			_new_rf_data = 1;
			
		}
		EXTI_ClearITPendingBit(EXTI_Line6);
	}

}


// the most important function of init_timer is to give a value to the lock pointer.
void init_timer (u8 *lock) {
 	//SysTick_Config (100);
	_milliseconds = 0;
	_wait_time = 0;
	timer_lock = lock;
}


// this timer is set up in gesture_latch.c
// used by the wait function
__irq void TIM3_IRQHandler(void){
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //clear the pending Interrupt
	_milliseconds++;

	if ( _milliseconds > _wait_time ) {
		*timer_lock = 0;
		TIM_Cmd(TIM3,DISABLE);

	}
}

// timer lock is set to 1 and tim3 is enabled. timer lock will become 0 once the wait time has been exceeded.
void wait(u16 wait_time, u8 *lock) {
   _milliseconds = 0;
   _wait_time = wait_time;
   timer_lock = lock;
   *timer_lock = 1;

	TIM_Cmd(TIM3,ENABLE);
}
// deprecate function
void wait_listen(u16 timeout) {

}



/*RF_CMD rf_wait_command (void) {

	rf_read_byte();	   */
/*	while(!_new_data);

	tmp = 0;
	while (tmp < 32)
		data_rf[tmp++] = read_byte(TI_CCxxx0_RXFIFO);
	
	_latest_byte = findMode(data_rf);

	send_strobe ( TI_CCxxx0_SIDLE);
	 
	send_strobe ( TI_CCxxx0_SFRX);

	return (RF_CMD)_latest_byte;  */	/*
	return 0;
}		   */

