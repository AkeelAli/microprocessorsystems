//include library header files
#include <stdio.h>


#include "rf2500.h"
#include "communication.h"
#include "protocol.h"
#include "gesture_latch.h"

symbol_t move_m;
RF_CMD received_command_m;
int main(void){
	 
	initAccGyro();
	initTIM();
	init_timer();
	init_protocol();
	init_spi();

	go_wait();
 

}

