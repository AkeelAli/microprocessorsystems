#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include <stdint.h>
#include "rf2500.h"

// definition of the various commands used for rf communicatoin
typedef enum {
	RF_CMD_NO_CMD	= 0x00,
	RF_CMD_REQ 		= 0x01, // 0000 0001 
	RF_CMD_ACK 		= 0x02,	// 0000 0010
	RF_CMD_ROCK 	= 0x04,	// 0000 0100
	RF_CMD_PAPER	= 0x10,	// 0001 0000
	RF_CMD_SCISSORS = 0x40,	// 0100 0000
	RF_CMD_MWIN 	= 0x08, // 0000 1000
	RF_CMD_MLOSE 	= 0x20,	// 0010 0000
	RF_CMD_TIE		= 0x80	// 1000 0000	
} RF_CMD;


// listen for a command
RF_CMD listen_command (void);

// send a command
void send_command(RF_CMD cmd);

// send followed by a listen
RF_CMD send_listen (RF_CMD cmd);

#endif
