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
	test_send();
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
	//wait ();

//	SPI_I2S_SendData( SPI1,(address | TI_CCxxx0_READ_SINGLE)<<8);
		

	while (SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE	) == RESET);

	SPI_I2S_SendData( SPI1,(address | TI_CCxxx0_READ_SINGLE)<<8);
	
   	t = SPI_I2S_ReceiveData(SPI1);

	raise_css();

	return t;
	

}

// this is weird. add wait
void test_send(void) { 		 
	write_byte (0x02,0x65);	  
	
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
