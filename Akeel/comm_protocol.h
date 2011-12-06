#ifndef COMM_PROTOCOL_H
#define COMM_PROTOCOL_H

#include "gestures.h"
#include "rf2500.h"

/* possible return values to main */
#define RETURN_WIN 0x01
#define RETURN_LOSS 0x02
#define RETURN_DRAW 0x03
#define RETURN_ERROR 0x04
#define RETURN_IN_PROGRESS 0x05


uint8_t communicate(symbol_t move);

#endif
