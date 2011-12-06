#include "communication.h"
RF_CMD listen_command (void) {	

 	return (RF_CMD)rf_read_byte(10000);
}

void send_command(RF_CMD cmd) {
	rf_send_byte(cmd);
}

RF_CMD send_listen (RF_CMD cmd) {

	rf_send_byte(cmd);
	return (RF_CMD)rf_read_byte(10000);

}








