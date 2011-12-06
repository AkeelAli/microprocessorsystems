
#include<stdio.h> 
#include "stm32f10x_conf.h"
struct __FILE { int handle;};
FILE __stdout;


int fputc (int ch, FILE *f) {
	ITM_SendChar(ch);
  	return ch;
}

int putchars(int ch){
 	return ITM_SendChar(ch);
}
