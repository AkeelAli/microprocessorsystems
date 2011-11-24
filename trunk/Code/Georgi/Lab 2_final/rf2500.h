#ifndef __RF2500_H
#define __RF2500_H

#include "stm32f10x_spi.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "C2500.h"
#include <stdbool.h>


void rf_transmit_packets ( u8 *byte, const u8 count);

void rf_receive_packets ( u8 *bytes, u8 count ) ;

// wait function, takes microseconds as input
void wait(void);

void raise_css (void);
void drop_css(void);

uint8_t miso_high (void);

void init_spi (void);


/// reset the chip
void rf_reset (void);

	//	TI_CCxxx0_TXFIFO | TI_CCxxx0_WRITE_BURST	  is the address byte



void test_send(void);
void write_byte (u16 address, u16 byte);
u8 read_byte (u16 address);

// sets a config register. it then reads back to make sure everything is fine
// return 0 on success and 1 on failure
u8 set_config (u16 address, u16 byte);
u8 configure (void);

#endif
