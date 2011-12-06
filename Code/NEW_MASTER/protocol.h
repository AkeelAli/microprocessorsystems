#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include <stdint.h>
#include "communication.h"
#include "iNEMO_Led.h"


// definition of states
typedef enum {
	RF_STATE_WAIT,	   			// both in this state; if sync gesture recorded go to RF_STATE_M_SYNC_SEND; if request received, go to 	RF_STATE_S_SYNC_WAIT

	RF_STATE_M_SYNC_SEND,		// become master; go to RF_STATE_M_MOVE_WAIT when acknowledge; next gesture recorded will be master's move
	RF_STATE_M_MOVE_WAIT,		// wait for slave to send its move; then go to RF_STATE_M_RESULT_SEND
	RF_STATE_M_RESULT_SEND,		// if no master move recorded, wait for master move; compute game outcome; send to slave; wait for slave to respond with same move, go to RF_STATE_M_END
	RF_STATE_M_END,				// send acknowledge and go to RF_STATE_WAIT
				   
	RF_STATE_S_SYNC_WAIT,		// become slave; go to RF_STATE_S_ACK_SEND after sync move recorded
	RF_STATE_S_ACK_SEND,		// next move recorded will be slave move; send acknowledge and go to RF_STATE_S_MOVE_SEND
	RF_STATE_S_MOVE_SEND,		// if no slave move recorded, wait for slave move; send move; go to	RF_STATE_S_END when result is received
	RF_STATE_S_END 				// send back result received; wait for acknowledge and go to wait
} RF_STATE;

// init protocol function. Enables the LED and interrupts on Line 11 (used for move latching)
void init_protocol(void);

// functions for the various states. master_* are master states, slave_* are slave states
void go_wait (void);

void master_sync_send (void);
void master_move_wait (void);
void master_result_send (uint8_t slave_cmd);
void master_end (uint8_t slave_cmd);


void slave_sync_wait (void);
void slave_acknowledge_send (void);
void slave_move_send (void);
void slave_end (uint8_t slave_cmd);

// the various LED actions to show outcome
void victory_dance(void);
void lose_and_weep (void);
void equality_for_all(void);

extern void Reset_Handler(void);  


#endif
