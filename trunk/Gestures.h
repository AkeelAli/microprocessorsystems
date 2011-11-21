//Amjad to update this with the right global_defs.h
#include <stdio.h>
#include <math.h>

//Threshold values use to indicate presence of microgesture:
#define		ACC_X_MAG	10
#define 	ACC_Y_MAG	10
#define		ACC_Z_MAG	10
#define		ROLL_MAG	25

typedef enum {
	acc_x,
	acc_y,
	acc_z,
	roll
} magn_max;

typedef enum {
	pos_x,
	neg_x,
	pos_y,
	neg_y,
	pos_z,
	neg_z,
	pos_roll,
	neg_roll
} micro_gestures;

typedef enum {
	paper,
	rock,
	scissor
} symbol;

s16 rawAcc[3];
s16 prev_rawAcc[3];
s16 rawGyr[3];
s16 prev_rawGyr[3];

int updateGesture(s16, s16, s16, s16);

