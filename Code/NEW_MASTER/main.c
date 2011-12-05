//include library header files
#include <stdio.h>


#include "rf2500.h"
#include "communication.h"
#include "protocol.h"
#include "gesture_latch.h"


int main(void){
	u8 lock = 0; 
	initAccGyro();
	
	init_timer(&lock);
	initTIM();
	init_protocol();
	init_spi();


	go_wait();
 

}

