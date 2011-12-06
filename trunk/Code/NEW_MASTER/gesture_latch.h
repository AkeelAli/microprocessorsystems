#ifndef __GESTURE_LATCH_H
#define __GESTURE_LATCH_H

#include "stm32f10x_conf.h"
#include "LSM303DLH.h"
#include "LPRYxxxAL.h"
#include "atan_LUT.h"
#include <math.h>
#include "gestures.h"

//Define constants/Macros
#define PI 3.141592654
extern symbol_t latched_move;
void initTIM(void);
void initAccGyro(void);
void update_Orientation(void);

symbol_t get_move (void);


// latch for a particular move. works for move combinations as well
void latch_move (symbol_t lmove);

#endif
