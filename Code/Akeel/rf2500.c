#include "rf2500.h"

u16 tmp;
u16 t,t2;
u8 t3;


void init_spi (void) {

	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

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

	//SysTick_Config (100);


	
	raise_css();


	rf_reset();
	
	configure();

	send_strobe	 (TI_CCxxx0_SFSTXON);
	send_strobe	 (TI_CCxxx0_SFSTXON);
	//send_strobe	 (TI_CCxxx0_SRX);



	//test_send();
//	SPI_I2S_SendData(SPI1, TI_CCxxx0_SRES);


}

void raise_css (void) {
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)1);
	while (!GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_4));		  // wait until bit actually changes

}
void drop_css(void){
  	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)0);
	while (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_4));	  // wait until bit actually changes
}


void rf_transmit_packets ( u8 *byte, const u8 count) {

	u8 i;
	drop_css();

	// wait for miso to go low
	while(miso_high());

	// write the address
	SPI_I2S_SendData  (SPI1,TI_CCxxx0_TXFIFO | TI_CCxxx0_WRITE_BURST);

	// write the data
	for (i = 0; i < count; i++)
		SPI_I2S_SendData  (SPI1,(uint16_t)byte[i]);

	// release

	raise_css(); 	

}

void rf_receive_packets ( u8 *bytes, u8 count ) {

	SPI_I2S_ReceiveData(SPI1);

}

RFStatus rf_get_status(void) {
 	return (RFStatus) (0x70 & send_strobe(TI_CCxxx0_SNOP));
}

void rf_reset (void) {

 	
	drop_css();
	raise_css();

	send_strobe	 (TI_CCxxx0_SRES);

//	t2=2000;
//	while(t2--)
//	t = SPI1->DR;
	
	
}



// this works!	 don't fucking touch anything
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
// this works!	 don't fucking DARE  touch anything
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


 u16 stats[100];
void rf_send_byte (u8 byte) {

	//switch ( rf_get_status () ) {
	 		
	/*	case RF_STATUS_RXFIFO_OVERFLOW:
			send_strobe	 (TI_CCxxx0_SFRX);

		case RF_STATUS_TXFIFO_UNDERFLOW:
			send_strobe	 (TI_CCxxx0_SFTX);

		case RF_STATUS_IDLE:
			
			send_strobe	 (TI_CCxxx0_SFSTXON);			
			send_strobe	 (TI_CCxxx0_SFSTXON);

		default:	 */
			
			   RFStatus status;

			 send_strobe	 (TI_CCxxx0_SIDLE);
			 while (rf_get_status() != RF_STATUS_IDLE);
			 send_strobe	 (TI_CCxxx0_SFTX);

			 send_strobe	 (TI_CCxxx0_SFSTXON);
			 tmp = 0;
			 while (tmp<64)
		     stats[tmp++] = write_byte( TI_CCxxx0_TXFIFO,byte);

			 while (rf_get_status() == RF_STATUS_CALIBRATE);


			  tmp = 100;
			  while(tmp--)
			status = rf_get_status();
			// if (rf_get_status() != RF_STATUS_FSTXON)
			// 	return;
			 send_strobe (TI_CCxxx0_STX);
			 while (rf_get_status() != RF_STATUS_TXFIFO_UNDERFLOW) {
			  	 while ((0x0F & write_byte( TI_CCxxx0_TXFIFO,byte)) > 0x01);
				  status = rf_get_status();
			 }

			 status = rf_get_status();

			 status = rf_get_status();
	//}


}
RFStatus st;
void test_send(void) { 		 

	u8 bytes[8];
	bytes[0] = 0x13;
	bytes[1] = 0x14;
	rf_send_byte(0x12);
	st = rf_get_status();
	while(rf_get_status() != RF_STATUS_TXFIFO_UNDERFLOW);

	rf_send_byte(0x12);

	while (1) st = rf_get_status();
	while (rf_get_status() == RF_STATUS_CALIBRATE);

	
	
		
	//send_strobe ( TI_CCxxx0_SFTX);

	
	tmp = 0;
  while(tmp < 63){
  	tmp++;
   	t3 = write_byte(TI_CCxxx0_TXFIFO,0x12);
	}
   t3 =   send_strobe(TI_CCxxx0_SNOP);
	send_strobe	 (TI_CCxxx0_STX);
    
//	t3 =	write_fifo(2,bytes);

//	t3 =   send_strobe(TI_CCxxx0_SNOP);
//	t3 =   send_strobe(TI_CCxxx0_SNOP);
    
    t3 =   send_strobe(TI_CCxxx0_SNOP);
	t3 =   send_strobe(TI_CCxxx0_SNOP);
	/*t3 = read_byte(TI_CCxxx0_READ_BURST | TI_CCxxx0_RXFIFO);
	t3 = read_byte(TI_CCxxx0_READ_BURST | TI_CCxxx0_RXFIFO);
	t3 = read_byte(TI_CCxxx0_READ_BURST | TI_CCxxx0_RXFIFO);
	t3 = read_byte(TI_CCxxx0_READ_BURST | TI_CCxxx0_RXFIFO);
	t3 = read_byte(TI_CCxxx0_READ_BURST | TI_CCxxx0_RXFIFO); */

}
FlagStatus s;
uint8_t miso_high (void) {

	return GPIO_ReadInputDataBit (GPIOA,GPIO_Pin_6);
	
}





u8 set_config (u16 address, u16 byte) {
 	 write_byte (address,byte);	  
	 
	 if (byte != read_byte(address))
	 	return 1;
	 else
		return 0;
}

u8 configure (void) {
		
	   while (set_config(TI_CCxxx0_IOCFG0,		SETTING_IOCFG0				)) ;
	   while (set_config(TI_CCxxx0_MCSM1,		SETTING_MCSM1				)) ;
	   while (set_config(TI_CCxxx0_MCSM0,		SETTING_MCSM0				)) ;
	   while (set_config(TI_CCxxx0_PKTLEN,		SETTING_PKTLEN				)) ;
	   while (set_config(TI_CCxxx0_PKTCTRL0,	SETTING_PKTCTRL0			)) ;	 
 	   while (set_config(TI_CCxxx0_FSCTRL1,		SMARTRF_SETTING_FSCTRL1		)) ;
	   while (set_config(TI_CCxxx0_IOCFG0,		SMARTRF_SETTING_FSCTRL0		)) ;
	   while (set_config(TI_CCxxx0_FREQ2,		SMARTRF_SETTING_FREQ2		)) ;
	   while (set_config(TI_CCxxx0_FREQ1,		SMARTRF_SETTING_FREQ1		)) ;
	   while (set_config(TI_CCxxx0_FREQ0,		SMARTRF_SETTING_FREQ0		)) ;
	   while (set_config(TI_CCxxx0_MDMCFG4,		SMARTRF_SETTING_MDMCFG4		)) ;
	   while (set_config(TI_CCxxx0_MDMCFG3,		SMARTRF_SETTING_MDMCFG3		)) ;
	   while (set_config(TI_CCxxx0_MDMCFG2,		SMARTRF_SETTING_MDMCFG2		)) ;
	   while (set_config(TI_CCxxx0_MDMCFG1,		SMARTRF_SETTING_MDMCFG1		)) ;
	   while (set_config(TI_CCxxx0_MDMCFG0,		SMARTRF_SETTING_MDMCFG0		)) ;
	   while (set_config(TI_CCxxx0_CHANNR,		0x13		)) ;
	   while (set_config(TI_CCxxx0_DEVIATN,		SMARTRF_SETTING_DEVIATN		)) ;
	   while (set_config(TI_CCxxx0_FREND1,		SMARTRF_SETTING_FREND1		)) ;
	   while (set_config(TI_CCxxx0_FREND0,		SMARTRF_SETTING_FREND0		)) ;
	   while (set_config(TI_CCxxx0_MCSM0,		SMARTRF_SETTING_MCSM0		)) ;
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
	   while (set_config(TI_CCxxx0_IOCFG2,		SMARTRF_SETTING_IOCFG2		)) ;
	   while (set_config(TI_CCxxx0_PKTCTRL1,	SMARTRF_SETTING_PKTCTRL1	)) ;
	   while (set_config(TI_CCxxx0_PKTCTRL0,	SMARTRF_SETTING_PKTCTRL0	)) ;
	   while (set_config(TI_CCxxx0_ADDR,		SMARTRF_SETTING_ADDR		)) ;
	   while (set_config(TI_CCxxx0_PKTLEN,		SMARTRF_SETTING_PKTLEN		)) ;

	   t3 = read_byte(TI_CCxxx0_CHANNR);
	   return 0;

}
