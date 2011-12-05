#include <math.h>
#include <stm32f10x.h>
#include "gestures.h"
#include "mgest_stack.h"

/* valid moves for the game */
gesture valid_moves[VALID_MOVES] = {
		 {{pos_x, neg_x}, scissors}, {{pos_y, neg_y}, paper}, {{pos_z, neg_z}, sync}, {{pos_roll, neg_roll}, rock},
		 {{neg_x, pos_x}, scissors}, {{neg_y, pos_y}, paper}, {{neg_z, pos_z}, sync}, {{neg_roll, pos_roll}, rock}
	};

int32_t prev_acc[3];
int32_t acc[3];
int32_t delta[3];
int32_t abs_delta[3];

int updateGesture(int32_t x, int32_t y, int32_t z, float roll, symbol_t *result) {

	acc[0] = x;
	acc[1] = y;
	acc[2] = z;

	delta[0] = (acc[0] - prev_acc[0]);
	delta[1] = (acc[1] - prev_acc[1]);
	delta[2] = (acc[2] - prev_acc[2]);

	abs_delta[0] = fabs(acc[0] - prev_acc[0]);
	abs_delta[1] = fabs(acc[1] - prev_acc[1]);
	abs_delta[2] = fabs(acc[2] - prev_acc[2]);
	
	/* roll overrides other gestures hence comes first in comparison */
	if (roll > ROLL_MAG)
		intelligent_push(pos_roll);

	else if (-roll > ROLL_MAG) 
		intelligent_push(neg_roll);

	else if (abs_delta[0] > abs_delta[1] && abs_delta[0] > abs_delta[2]) {
		if (delta[0] > ACC_X_MAG)			 
			intelligent_push(pos_x);
		else if (-delta[0] > ACC_X_MAG)
			intelligent_push(neg_x);	
	}
	else if (abs_delta[1] > abs_delta[0] && abs_delta[1] > abs_delta[2]) {
		if (delta[1] > ACC_Y_MAG)		
			intelligent_push(pos_y);
		else if (-delta[1] > ACC_Y_MAG)
			intelligent_push(neg_y);
	}
	else {
		if (delta[2] > ACC_Z_MAG)			
			intelligent_push(pos_z);
		else if (-delta[2] > ACC_Z_MAG)
			intelligent_push(neg_z);	
	} 
	
	//Update Prev_values:
	prev_acc[0] = acc[0];
	prev_acc[1] = acc[1];
	prev_acc[2] = acc[2];

	//Calling Process_symbol (if 2 mgests on stack)
	return process_symbol(result);
}
