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

void initTIM(void);
void initAccGyro(void);
void update_Orientation(void);

symbol_t get_move (void);

void latch_move (symbol_t lmove);

#endif
