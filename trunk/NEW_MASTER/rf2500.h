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

// various statuses
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

// function to get the current status of the chip
RFStatus rf_get_status(void);

// function to get the free bytes in the write buffer
u8 rf_get_free_bytes(void);

// function to initialize the timer. needs to be called before initTIM();
void init_timer(u8 *lock);

// wait function, takes tens of microseconds as input. the lock variable will be changed to 0 on timeout
void wait(u16 wait_time, u8 *lock);

//deprecated function
void wait_listen(u16 timeout)  ;

// functions to raise and to lower the chip select line of the spi
void raise_css (void);
void drop_css(void);

// function to check if MISO is high
uint8_t miso_high (void);

// function that initializes the spi. enables the clocks, various gpios,
// the interrupt on line 6, resets the rf chip and configures it
void init_spi (void);


// reset the chip
void rf_reset (void);

	//	TI_CCxxx0_TXFIFO | TI_CCxxx0_WRITE_BURST	  is the address byte


// deprecated function
void test_send(void);

// this function writes a byte to the specified address (spi)
u16 write_byte (u16 address, u16 byte);

// reads a byte from the specified address
u16 read_byte (u16 address);

// sets a config register. it then reads back to make sure everything is fine
// return 0 on success and 1 on failure
u8 set_config (u16 address, u16 byte);

// sets all the configs. values are in C2500.h
u8 configure (void);

// send a command strobe to the rf chip
u16 send_strobe (u8 strobe);


// send a byte using the rf chip
void rf_send_byte (u8 byte);

// receive a byte using the rf chip
u8 rf_read_byte (u16 waittime);

#endif
