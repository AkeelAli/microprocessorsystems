#include "spi_protocol.h"
#include "cc2500.h"
	

void drop_css()
{
	// Set the slave select to zero
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)0);
	// Wait until the slave select goes down
	while (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_4));
}


void raise_css()
{
	// Set the slave select to 1
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)1);
	// Wait until the slave select goes up
	while (!GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_4));
}


void SPI_Write(uint16_t address, uint16_t message)
{
	// Concatenate the address and the message
	uint16_t transmission = (address << 8) | message;
	
	drop_css();
	SPI_I2S_SendData(SPI1, transmission);

	// Clear the buffer by reading from it
	drop_css();
	SPI_I2S_ReceiveData(SPI1);
	
	// Set the pin back to high after the transaction is finished
	raise_css();
}


uint16_t SPI_Read(uint16_t address)
{
	// Make the MSB a 1 to make it a read. Also, just make it have the address twice.
	uint16_t transmission = 0x8000 | (address << 8 ) | address;

	// Send the address you want to read from
	drop_css();
	SPI_I2S_SendData(SPI1, transmission); 
	
	// Read
	drop_css();
	transmission = SPI_I2S_ReceiveData(SPI1);

	// Set the pin back to highafter the transaction is finished
	raise_css();
	
	return transmission;	
}


// Send the command defined by "address" to the CC2500 board
void SPI_Command(char address)
{
	// Make the data size 8 bits instead of 16 because commands are only 8 bits
	SPI_DataSizeConfig(SPI1, SPI_DataSize_8b);

	drop_css();
	SPI_I2S_SendData(SPI1, address);

	drop_css();
	SPI_I2S_ReceiveData(SPI1);

	GPIO_SetBits(GPIOA, GPIO_Pin_4);
		
	// Set the data format back to 16 bits	 
	SPI_DataSizeConfig(SPI1, SPI_DataSize_16b);
}




void SPI_Configuration()
{
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

	raise_css();
}
