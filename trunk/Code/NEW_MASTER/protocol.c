#include "protocol.h"
#include "gesture_latch.h"

RF_STATE _current_state = RF_STATE_WAIT;
RF_CMD received_command = RF_CMD_NO_CMD;
RF_CMD master_move = RF_CMD_NO_CMD;
RF_CMD slave_move = RF_CMD_NO_CMD;
RF_CMD game_result = RF_CMD_NO_CMD;

Led_TypeDef led; 

symbol_t recorded_move = no_move;

void init_protocol(void) {

	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	iNEMO_Led_Init(led);

	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0A;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
	
	

}

__irq void EXTI15_10_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line11) != RESET) {
		recorded_move = get_move();
		EXTI_ClearITPendingBit(EXTI_Line11);
	}

}

void go_wait (void) {
	recorded_move = no_move;
	latch_move (sync);
	 
   _current_state = RF_STATE_WAIT;	  
	while (	RF_CMD_REQ != received_command && sync != recorded_move) {
		received_command = listen_command();
	}
	if (sync == recorded_move) {
		recorded_move = no_move;
		
		master_sync_send();

	}
	else	 {
		slave_sync_wait();
	}
}



void master_sync_send (void) {
	uint8_t i = 0;

		recorded_move = no_move;
		master_move = RF_CMD_NO_CMD;
		latch_move (rock | paper | scissors);
	
	
	_current_state = RF_STATE_M_SYNC_SEND;
	while (i < 20 && 	RF_CMD_NO_CMD == received_command ) {
		received_command =  send_listen (RF_CMD_REQ);
		i++;
	}
	if  (RF_CMD_ACK == received_command) {
		master_move_wait();
	}
	else {
		go_wait();
	}
}

void master_move_wait (void) {
	uint8_t i = 0;
 	_current_state = RF_STATE_M_MOVE_WAIT;

	while ((i < 50) && !((RF_CMD_ROCK | RF_CMD_PAPER | RF_CMD_SCISSORS) & received_command )) {
		received_command =  listen_command ();
		//received_command = rf_read_byte (10000);
		i++;
		
	}
	if (((RF_CMD_ROCK | RF_CMD_PAPER | RF_CMD_SCISSORS) & received_command ))	{
	 	  master_result_send (received_command);
	}
	else {
		go_wait();
	}
}

void master_result_send (uint8_t slave_cmd) {

	uint8_t i = 0;
	RF_CMD game_result = RF_CMD_NO_CMD;

	_current_state = RF_STATE_M_RESULT_SEND;
	

	while (RF_CMD_NO_CMD == master_move && i++ < 10) {
		while (no_move == recorded_move);
	
			switch (recorded_move) {
			 	case rock:
					 master_move = RF_CMD_ROCK;
					 break;
				case paper:
					master_move = RF_CMD_PAPER;
					break;
				case scissors:
					master_move = RF_CMD_SCISSORS;
					break;
				default:
					master_move = 	RF_CMD_NO_CMD;		 
			}
			recorded_move = no_move;
		
	}
	
	switch (master_move) { 	 	
		case RF_CMD_ROCK:
			if ( RF_CMD_ROCK == (RF_CMD)slave_cmd) 
				game_result = RF_CMD_TIE;
			else if ( RF_CMD_PAPER == (RF_CMD)slave_cmd) 
				game_result = RF_CMD_MLOSE;
			else
				game_result = RF_CMD_MWIN;

			break;

		case RF_CMD_PAPER:

			if ( RF_CMD_PAPER == (RF_CMD)slave_cmd) 
				game_result = RF_CMD_TIE;
			else if ( RF_CMD_SCISSORS == (RF_CMD)slave_cmd) 
				game_result = RF_CMD_MLOSE;
			else
				game_result = RF_CMD_MWIN;

			break;

		case RF_CMD_SCISSORS:
			if ( RF_CMD_SCISSORS == (RF_CMD)slave_cmd) 
				game_result = RF_CMD_TIE;
			else if ( RF_CMD_ROCK == (RF_CMD)slave_cmd) 
				game_result = RF_CMD_MLOSE;
			else
				game_result = RF_CMD_MWIN; 
			
			break;
		default:
			go_wait();
			break;
	}
	i = 0;
	while (i < 30 && game_result != received_command ) {
		received_command = send_listen (game_result);
		i++;
	}

	if (game_result == received_command) {
		master_end(game_result);		
	}

	else {
	 	go_wait();
	}

}

void master_end (uint8_t slave_cmd) {
	uint8_t i = 0;
	 game_result = (RF_CMD) slave_cmd;
	_current_state = RF_STATE_M_END;
	while (i++ < 5)
		send_command(RF_CMD_ACK);

	switch(game_result) {
		 	case RF_CMD_MLOSE:
				lose_and_weep();
				break;
			case RF_CMD_MWIN:
				victory_dance();
				break;
			case RF_CMD_TIE:
				equality_for_all();
				break;

	}

	go_wait();
}





void slave_sync_wait (void) {

	_current_state = RF_STATE_S_SYNC_WAIT;
 	
	recorded_move = no_move;
	//latch_move (sync);

//	while (sync != recorded_move);

	
	// some kind of timeout

	// if (timeout) go_wait();

	recorded_move = no_move;
	slave_acknowledge_send();

}

void slave_acknowledge_send (void) {
	uint8_t i = 0;
	_current_state = RF_STATE_S_ACK_SEND;
	 slave_move = RF_CMD_NO_CMD;
	 recorded_move = no_move;
	 latch_move (rock | paper | scissors);

	

	while (i++ < 2)
		send_command(RF_CMD_ACK);
	slave_move_send();


}

void slave_move_send (void) {
	uint8_t i = 0;

	_current_state = RF_STATE_S_MOVE_SEND;

	
	while (RF_CMD_NO_CMD == slave_move && i++ < 10) {
		if (no_move == recorded_move) {
			wait (10000);
		}
		else  {
			switch (recorded_move) {
			 	case rock:
					 slave_move = RF_CMD_ROCK;
					 break;
				case paper:
					slave_move = RF_CMD_PAPER;
					break;
				case scissors:
					slave_move = RF_CMD_SCISSORS;
					break;			 
			}
			recorded_move = no_move;
		}
	}




	while ((i < 50) && !((RF_CMD_MWIN | RF_CMD_MLOSE | RF_CMD_TIE) & received_command ) ) {
		received_command = send_listen (slave_move);
		//send_command(slave_move);
		i++;

	}

	if (((RF_CMD_MWIN | RF_CMD_MLOSE | RF_CMD_TIE) & received_command ))	{
	 	  slave_end (received_command);
	}
	else {
		go_wait();
	}
}

void slave_end (uint8_t slave_cmd) {
	
	uint8_t i = 0;
	 game_result = (RF_CMD) slave_cmd;
	_current_state = RF_STATE_S_END;
 	
	while (i < 30 && RF_CMD_ACK != received_command ) {
		received_command = send_listen ((RF_CMD)slave_cmd);
		i++;
	}
	if (RF_CMD_ACK == received_command)	{

		switch(game_result) {
		 	case RF_CMD_MWIN:
				lose_and_weep();
				break;
			case RF_CMD_MLOSE:
				victory_dance();
				break;
			case RF_CMD_TIE:
				equality_for_all();
				break;

		}

	 	go_wait();
	}
	else {
		go_wait();
	}
}

void victory_dance(void) {
		uint8_t i = 0;

		while (i++ < 50) {
			iNEMO_Led_Toggle(led);
			wait(10000);
		}
}

void lose_and_weep (void) {

	uint8_t i = 0;
		while (i++ < 50) {
			iNEMO_Led_On(led);
			wait(5000);
			

		}	
		iNEMO_Led_Off(led);

}

void equality_for_all (void) {

	uint8_t i = 0;

		while (i++ < 20) {
			iNEMO_Led_On(led);
			wait(40000);
			iNEMO_Led_Off(led);
			wait(5000);

		}

}
