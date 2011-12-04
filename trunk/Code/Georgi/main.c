//include library header files
#include <stdio.h>


#include "rf2500.h"
#include "communication.h"
#include "protocol.h"
#include "gesture_latch.h"

symbol_t move_m;
int main(void){
	
	//initialize Accelerometer	and Gyroscope
	initAccGyro();
	//initialize the Timer Interrupt
	initTIM();
	//initialize SPI & RF
	init_timer();
	init_protocol();
	init_spi();
	slave_acknowledge_send();
	//master_sync_send();
	go_wait();
	
//	 while(1) move_m = get_move();
	//rf_wait_command();		
	/*					
	while(1){
	  if(newData){
		 update_Orientation();
	
		 if (updateGesture(accelX, accelY, accelZ, roll_fuse, &move) == SUCCESS);
		
		 //write_byte (0x2C, 0x56);
		 //readout = read_byte (0x2C);
		 //rf_send_byte(0x17);					 

		 newData = 0;
	 } } 
	   */
	 
		 

}

