//include library header files
#include <stdio.h>


#include "rf2500.h"
#include "communication.h"
#include "protocol.h"
#include "gesture_latch.h"


int main(void){
				  extern u8 latching;
	initAccGyro();
	initTIM();
	init_timer();
	init_protocol();
	init_spi();
	
	go_wait();
 

}

