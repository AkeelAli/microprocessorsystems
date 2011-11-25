#ifndef GESTURES_H
#define GESTURES_H

//Threshold values use to indicate presence of microgesture:
#define		ACC_X_MAG	3000
#define 	ACC_Y_MAG	2000
#define		ACC_Z_MAG	7000
#define		ROLL_MAG	10	  /* threshold angle */

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

typedef struct {
	mgest_t	 mgest[2];
	symbol_t symbol; 	
} gesture;	/* macro gestures: symbol = mgest1 + mgest2 */

#define VALID_MOVES 8
extern gesture valid_moves[VALID_MOVES]; 

#include <stm32f10x.h>

int updateGesture(float x, float y, float z, float roll, symbol_t *result);

#endif
