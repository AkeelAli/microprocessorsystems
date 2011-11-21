#include <math.h>
#include <stm32f10x.h>
#include "gestures.h"
#include "mgest_stack.h"

/* valid_moves and moves go together, and must be updated together */
mgest_t valid_moves[VALID_MOVES][2] = { 
		{pos_x, neg_x}, {pos_y, neg_y}, {pos_z, neg_z}, {pos_roll, neg_roll},
		{neg_x, pos_x}, {neg_y, pos_y}, {neg_z, pos_z}, {neg_roll, pos_roll}
	}; 
symbol_t moves[VALID_MOVES] = {scissors, paper, sync, rock, scissors, paper, sync, rock};

static float prev_acc[3];
static float acc[3];
static float delta[3];

int updateGesture(float x, float y, float z, float roll, symbol_t *result) {

	acc[0] = x;
	acc[1] = y;
	acc[2] = z;

	delta[0] = fabs( (acc[0] - prev_acc[0]) );
	delta[1] = fabs( (acc[1] - prev_acc[1]) );
	delta[2] = fabs( (acc[2] - prev_acc[2]) );
	
	/* roll overrides other gestures hence comes first in comparison */
	if (roll > ROLL_MAG)
		intelligent_push(pos_roll);

	else if (-roll > ROLL_MAG) 
		intelligent_push(neg_roll);

	else if (delta[0] > delta[1] && delta[0] > delta[2]) {
		if (acc[0] > ACC_X_MAG)			 /* compare the value as opposed to the delta */
			intelligent_push(pos_x);
		else if (-acc[0] > ACC_X_MAG)
			intelligent_push(neg_x);	
	}
	else if (delta[1] > delta[0] && delta[1] > delta[2]) {
		if (acc[1] > ACC_Y_MAG)			 /* compare the value as opposed to the delta */
			intelligent_push(pos_y);
		else if (-acc[1] > ACC_Y_MAG)
			intelligent_push(neg_y);
	}
	else {
		if (acc[2] > ACC_Z_MAG)			 /* compare the value as opposed to the delta */
			intelligent_push(pos_z);
		else if (-acc[2] > ACC_Z_MAG)
			intelligent_push(neg_z);	
	} 
	
	//Update Prev_values:
	prev_acc[0] = acc[0];
	prev_acc[1] = acc[1];
	prev_acc[2] = acc[2];

	//Calling Process_symbol (if 2 mgests on stack)
	return process_symbol(result);
}
