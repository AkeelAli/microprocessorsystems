#include "gestures.h"
#include "mgest_stack.h"

#define MAX 2

static mgest_t buff[MAX];
static int sp = 0;	 /* next free space on stack */

static int isEmpty(){
	return (sp == 0);
}

static int isFull(){
	return (sp == MAX);
}

int push(mgest_t g) {
	if (isFull())
		return ERROR;
	
	buff[sp++] = g;
	return SUCCESS;
}

int pop(mgest_t *g) {
	if (isEmpty())
		return ERROR;
	
	*g = buff[--sp];
	return SUCCESS;
}

int peek(mgest_t *g){
	if (isEmpty())
		return ERROR;
	
	*g = buff[sp-1];
	return SUCCESS;	
}

/* pushes micro gesture only if it's not at the top of the stack */
int intelligent_push(mgest_t g) {
	mgest_t head;

	if(!isEmpty())	/* only peek if stack not empty */
		if( peek(&head) == ERROR || head == g) /* don't push if empty or same */
			return ERROR;

	push(g);
	return SUCCESS;
}

/* checks stack for well formed gesture */
int process_symbol(symbol_t *s){
	mgest_t tmp1, tmp2;
	
	if(sp != 2)  /* must have 2 mgests on stack */
		return ERROR;
	
	if ((pop(&tmp1) != ERROR) && (pop(&tmp2) != ERROR)){
	 	int i;
		for (i = 0; i < VALID_MOVES; i++){
			if (tmp1 == valid_moves[i][0] && tmp2 == valid_moves[i][1]){
				*s = moves[i];
				return SUCCESS;
			}
		}
	}

	//re-push only the last-in element (no valid move)
	push(tmp1);
	s = 0;	// 0 = NULL
	return ERROR;
}
