#ifndef _SPI_PROTOCOL_H_
#define _SPI_PROTOCOL_H_

#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

void drop_css(void);
void raise_css(void);
void SPI_Write(uint16_t address, uint16_t message);
uint16_t SPI_Read(uint16_t address);
void SPI_Command(char address);
void GPIO_SPI_Configuration(void);
void SPI_Configuration(void);	 

#endif	 // _SPI_PROTOCOL_H_
