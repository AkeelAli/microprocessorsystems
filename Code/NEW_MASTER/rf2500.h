#ifndef __RF2500_H
#define __RF2500_H

#include "stm32f10x_spi.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "C2500.h"
#include <stdbool.h>

#include "service_functions.h"
#include "protocol.h"
#include "communication.h"


typedef enum {

	RF_STATUS_IDLE = 0x00, 
	RF_STATUS_RX = 0x10,
	RF_STATUS_TX = 0x20,
	RF_STATUS_FSTXON = 0x30,
	RF_STATUS_CALIBRATE = 0x40,
	RF_STATUS_SETTING = 0x50,
	RF_STATUS_RXFIFO_OVERFLOW = 0x60,
	RF_STATUS_TXFIFO_UNDERFLOW	= 0x70
} RFStatus;

RFStatus rf_get_status(void);
u8 rf_get_free_bytes(void);

void init_timer(u8 *lock);
// wait function, takes tens of microseconds as input
void wait(u16 wait_time, u8 *lock);

void wait_listen(u16 timeout)  ;
void raise_css (void);
void drop_css(void);

uint8_t miso_high (void);

void init_spi (void);


/// reset the chip
void rf_reset (void);

	//	TI_CCxxx0_TXFIFO | TI_CCxxx0_WRITE_BURST	  is the address byte



void test_send(void);
u16 write_byte (u16 address, u16 byte);

u16 read_byte (u16 address);

// sets a config register. it then reads back to make sure everything is fine
// return 0 on success and 1 on failure
u8 set_config (u16 address, u16 byte);
u8 configure (void);

u16 send_strobe (u8 strobe);

void rf_send_byte (u8 byte);
u8 rf_read_byte (u16 waittime);

#endif
