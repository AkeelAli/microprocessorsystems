#ifndef GESTURES_H
#define GESTURES_H

//Threshold values use to indicate presence of microgesture:
#define		ACC_X_MAG	10000000
#define 	ACC_Y_MAG	10000000
#define		ACC_Z_MAG	10000000
#define		ROLL_MAG	25	  /* threshold angle */

typedef enum {
	pos_x = 1,
	neg_x,
	pos_y,
	neg_y,
	pos_z,
	neg_z,
	pos_roll,
	neg_roll
} mgest_t; /* micro gesture (eg. forward x, backward x, etc.) */

typedef enum {
	paper = 1,
	rock,
	scissors,
	sync
} symbol_t;	

#define VALID_MOVES 8
extern mgest_t valid_moves[VALID_MOVES][2]; 
extern symbol_t moves[VALID_MOVES];

#include <stm32f10x.h>

int updateGesture(float x, float y, float z, float roll, symbol_t *result);

#endif
