//include library header files
#include <stdio.h>


#include "rf2500.h"
#include "communication.h"
#include "protocol.h"
#include "gesture_latch.h"


int main(void){
	
	initAccGyro();
	initTIM();
	init_timer();
	init_protocol();
	init_spi();

	equality_for_all();
	go_wait();
 

}

