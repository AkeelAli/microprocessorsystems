#include "rf2500.h"




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


	// need to define the MISO here somewhere


	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_SSOutputCmd(SPI1, ENABLE);
	
	SPI_Cmd(SPI1, ENABLE);

	SysTick_Config (800);
	rf_reset();

	test_send();
	configure();
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

void rf_reset (void) {

 	
	raise_css();
	drop_css();
	raise_css();
	
	wait();

	drop_css();

	while(miso_high());

	SPI_I2S_SendData(SPI1, TI_CCxxx0_SRES<<8);


	raise_css();
	
}
u16 tmp;
u16 t,t2;
u8 t3;

// this works!	 don't add wait
void write_byte (u16 address, u16 byte) {
	
	drop_css();

	while(miso_high());


	SPI_I2S_SendData  (SPI1,address << 8 | byte);

	raise_css();
}

u8 read_byte (u16 address) {
	t=0;
	t2=1;
	drop_css();
	while(miso_high());
	wait ();

	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == RESET);
	// keep reading untill 200 times the same value
   	tmp = 2000;
  	while (tmp){
		SPI_I2S_SendData( SPI1,(address | TI_CCxxx0_READ_SINGLE)<<8);
		
	   	t = SPI_I2S_ReceiveData(SPI1);
		if (t2 == t)
			tmp--;
		else
			tmp = 2000;
		t2 = t;
	}
	raise_css();
//	wait();
	return t;
	

}

// this is weird. add wait
void test_send(void) { 		 
	write_byte (0x02,0x12);	  
	
	t3 = read_byte(0x02);   

}
FlagStatus s;
uint8_t miso_high (void) {

	return GPIO_ReadInputDataBit (GPIOA,GPIO_Pin_6);
	
}

bool _timer_lock = true;
bool _called_once = false;
void wait(void) {

   _timer_lock = true;
	while (_timer_lock);
}

void SysTick_Handler(void) {
	if (_timer_lock){
		_called_once = true;
	}
	if (_called_once) {
		_timer_lock = false; 
		_called_once = false;
	}

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
	   while (set_config(TI_CCxxx0_CHANNR,		SMARTRF_SETTING_CHANNR		)) ;
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

	   t3 = read_byte(TI_CCxxx0_IOCFG0);
	   return 0;

}
