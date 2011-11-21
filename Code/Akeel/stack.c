#include "Gestures.h"

#define MAX 10

micro_gesture buff[MAX];
char sp = 0;

int push(micro_gesture g){
	if (sp == MAX)
		return -1;
	
	buff[sp++] = g;
	return 1;
}

int pop(micro_gesture *g){
	if (sp == 0)
		return -1;
	
	*g = buff[sp--];
	return 1;
}