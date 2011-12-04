#include "comm_protocol.h"

#define TIMEOUT 300

/* win status of the game (internal)*/
#define WIN_STATUS_IN_PROGRESS 0x00
#define WIN_STATUS_MASTER_WIN 0x01
#define WIN_STATUS_SLAVE_WIN 0x02
#define WIN_STATUS_DRAW (WIN_STATUS_MASTER_WIN | WIN_STATUS_SLAVE_WIN) //0x03

/* enums and structs */
typedef enum {
	CMD_INVALID = 0x00,
	
	CMD_REQ = 0x01,
	CMD_ACK = 0x02,
	
	CMD_ROCK = 0x04,
	CMD_PAPER = 0x08,
	CMD_SCISSORS = 0x10,
	CMD_SYNC = 0x20,
	
	CMD_WIN = 0x40,
	CMD_LOSE = 0x80
	
} command_t;

typedef enum {
	WAIT = 0x00,
	
	SLAVE_SYNC_WAIT = 0x01,
	
	MASTER_TX_ASSERT_RX_ACK = 0x02,	  //register master move in parallel
	SLAVE_TX_ACK = 0x03,	//transmit ACK long enough (do it over 2 states)
	SLAVE_TX_ACK_WAIT_MOVE = 0x04, 
	
	SLAVE_TX_MOVE_RX_RESULT_1 = 0x05, //abort after dT
	MASTER_RX_MOVE = 0x06,
	
	MASTER_TX_RESULT_RX_RESULT = 0x07, //register master move in parallel
	SLAVE_TX_RESULT_RX_ACK_1 = 0x08,
	MASTER_TX_ACK = 0x09,

	SLAVE_TX_MOVE_RX_RESULT_2 = 0x0A,
	SLAVE_TX_RESULT_RX_ACK_2 = 0x0B,
	MASTER_FINAL = 0x0C,
	SLAVE_FINAL	= 0x0D
	
} state_t;


typedef enum {
	UNDEFINED = 0x00,
	MASTER = 0x01,
	SLAVE = 0x02
} role_t;

static struct {
	role_t role;
	command_t master_move;
	command_t slave_move ;
	uint8_t win_status ; 
	uint16_t game_iterations ;
} data;

/* global vars */
state_t state = WAIT;
role_t role = UNDEFINED;


/* alternates tx/rx, and if it receives what it expects from rx, it returns it 
AND updates state to the next */
command_t 
dual_tx_rx(command_t tx, command_t expected_rx, state_t next, state_t abort) {
	static uint16_t tx_rx_iterations = TIMEOUT;
	command_t actual_rx;
	
	if (tx_rx_iterations-- == 0) {
		state = abort;
		if (state == WAIT)
			role = UNDEFINED;
		tx_rx_iterations = TIMEOUT;
	}
	else {
		/* receive 5 times more than you send */
		if (tx_rx_iterations % 5) {
			/* & with rx in case rx is one of the result command_t */
			if ((actual_rx = (command_t) rf_read_byte()) & expected_rx) {
				state = next;
				tx_rx_iterations = TIMEOUT;
				return actual_rx;
			}
		}
		else
			rf_send_byte(tx);
	}
	
	return CMD_INVALID;
}

command_t map_symbol_cmd(symbol_t move) {
	if (move == paper) return CMD_PAPER;
	if (move == rock) return CMD_ROCK;
	if (move == scissors) return CMD_SCISSORS;
	return CMD_SYNC;
}

/* registers first master/slave move after sync */
static void register_move(command_t* to_update, symbol_t move) {
	if (*to_update == CMD_INVALID && move != sync)
		*to_update = map_symbol_cmd(move);
}

static void determine_winner() {
	if (!data.win_status && data.slave_move && data.master_move) {
		/* default both win (eliminates 3/9 cases) */
		data.win_status = WIN_STATUS_DRAW;
		switch (data.slave_move) {
			case CMD_ROCK:
				switch (data.master_move) {
					case CMD_PAPER:
						data.win_status = WIN_STATUS_MASTER_WIN ;
						return;
					case CMD_SCISSORS:
						data.win_status = WIN_STATUS_SLAVE_WIN ;
						return;
				}
			case CMD_PAPER:
				switch (data.master_move) {
					case CMD_SCISSORS:
						data.win_status = WIN_STATUS_MASTER_WIN ;
						return;
					case CMD_ROCK:
						data.win_status = WIN_STATUS_SLAVE_WIN ;
						return;
				}
			case CMD_SCISSORS:
				switch (data.master_move) {
					case CMD_ROCK:
						data.win_status = WIN_STATUS_MASTER_WIN ;
						return;
					case CMD_PAPER:
						data.win_status = WIN_STATUS_SLAVE_WIN ;
						return;
				}
		}
	}
}

/* reset data at start of game (when assigned new role MASTER or SLAVE) */
void reset_data() {
	data.role = UNDEFINED;
	data.master_move = CMD_INVALID;
	data.slave_move = CMD_INVALID;
	data.win_status = WIN_STATUS_IN_PROGRESS;
	data.game_iterations = 0;
}


uint8_t communicate(symbol_t move) {
	command_t tmp_command;

	if (role == MASTER) {
		switch(state) {
			case MASTER_TX_ASSERT_RX_ACK :
				register_move(&data.master_move, move);
				dual_tx_rx(CMD_REQ, CMD_ACK, MASTER_RX_MOVE, WAIT);
				break;
			case MASTER_RX_MOVE :
				register_move(&data.master_move, move);
				if ((data.slave_move = (command_t) rf_read_byte()) & CMD_ROCK & CMD_PAPER & CMD_SCISSORS)
					state = MASTER_TX_RESULT_RX_RESULT;
				else
					data.slave_move = CMD_INVALID;
				break;
			case MASTER_TX_RESULT_RX_RESULT :
				register_move(&data.master_move, move);
				if (data.master_move) {
					determine_winner();
					dual_tx_rx(data.win_status == WIN_STATUS_SLAVE_WIN ? CMD_WIN:CMD_LOSE, 
						data.win_status == WIN_STATUS_SLAVE_WIN ? CMD_WIN:CMD_LOSE, MASTER_TX_ACK, WAIT);
				}
				break;
			case MASTER_TX_ACK :
				rf_send_byte(CMD_ACK);
				state = MASTER_FINAL;
				break;
			case MASTER_FINAL :
				state = WAIT;
				/* return final state of master */
				if (data.win_status == WIN_STATUS_DRAW ) return RETURN_DRAW;
				if (data.win_status == WIN_STATUS_SLAVE_WIN ) return RETURN_LOSS;
				if (data.win_status == WIN_STATUS_MASTER_WIN ) return RETURN_WIN;
				else return RETURN_ERROR;
		}
	}
	else if (role == SLAVE) {
		switch(state) {
			case SLAVE_SYNC_WAIT:
				if (move == sync)
					state = SLAVE_TX_ACK;
				break;
			/* send ACK twice (maybe need more) */
			case SLAVE_TX_ACK :
				register_move(&data.slave_move, move);
				rf_send_byte(CMD_ACK);	//maybe repeat this send a few times here
				state = SLAVE_TX_ACK_WAIT_MOVE;
				break;
			case SLAVE_TX_ACK_WAIT_MOVE :
				register_move(&data.slave_move, move);
				rf_send_byte(CMD_ACK);
				if (data.slave_move)
					state = SLAVE_TX_MOVE_RX_RESULT_1;
				break;
				
			/* process below repeats twice in case we get wrong result first time */
			case SLAVE_TX_MOVE_RX_RESULT_1 :
				//intended assignment
				if (tmp_command = dual_tx_rx(data.slave_move, (command_t) (CMD_WIN | CMD_LOSE), SLAVE_TX_RESULT_RX_ACK_1, WAIT))
					if (tmp_command == CMD_WIN)
						data.win_status = WIN_STATUS_SLAVE_WIN ;
					else if (tmp_command == CMD_LOSE)
						data.win_status = WIN_STATUS_MASTER_WIN ;

				break;
			case SLAVE_TX_MOVE_RX_RESULT_2 :
				if (tmp_command = dual_tx_rx(data.slave_move, (command_t) (CMD_WIN | CMD_LOSE), SLAVE_TX_RESULT_RX_ACK_2, WAIT))
					/* takes care of draw too */
					if (tmp_command & CMD_WIN)
						data.win_status |= WIN_STATUS_SLAVE_WIN ;
					if (tmp_command & CMD_LOSE)
						data.win_status |= WIN_STATUS_MASTER_WIN ;
				break;
			case SLAVE_TX_RESULT_RX_ACK_1 :
				/* will go back to previous state if no ack the first time */
				dual_tx_rx(data.win_status & WIN_STATUS_SLAVE_WIN ? CMD_WIN:CMD_LOSE, CMD_ACK, SLAVE_FINAL, SLAVE_TX_MOVE_RX_RESULT_2);
				break;
			case SLAVE_TX_RESULT_RX_ACK_2 :
				/* will reset if no ack the second time around */
				dual_tx_rx(data.win_status & WIN_STATUS_SLAVE_WIN ? CMD_WIN:CMD_LOSE, CMD_ACK, SLAVE_FINAL, WAIT);
				break;	
	
			case SLAVE_FINAL :
				state = WAIT;
				/* return final state of slave */
				if (data.win_status == WIN_STATUS_DRAW) return RETURN_DRAW;
				if (data.win_status == WIN_STATUS_MASTER_WIN) return RETURN_LOSS;
				if (data.win_status == WIN_STATUS_SLAVE_WIN) return RETURN_WIN;
				else return RETURN_ERROR;				
		}
	}
	/* UNDEFINED */
	else if (move == sync) { 
		reset_data();
		role = MASTER;
		state = MASTER_TX_ASSERT_RX_ACK;
	}
	else if (rf_read_byte() ==  CMD_REQ) {
		reset_data();
		role = SLAVE;
		rf_send_byte(CMD_ACK);
	}
	
	data.game_iterations++;
	return RETURN_IN_PROGRESS;
}
