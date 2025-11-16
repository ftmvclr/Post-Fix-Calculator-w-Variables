#include <stdio.h>
#include <string.h>
#include <math.h>

enum operator {MULTIPLICATION = 42, ADDITION, SUBTRACTION = 45, DIVISION = 47, EQUALS = 61, EXPONENT = 94};
struct element{
	int value; // the value if it is an operand, -1 if not 
	char op; // the char if it is an operator, -1 if not an operator (EOF)
	int is_known; // 0 means variable (letter), 1 means "value or op" is not -1
	struct element *bottom; // for the stack
};
typedef struct element Element;


// pointer to char array
int main(int argc, char *argv[]){
	Element *headPtr = NULL;
	printf("successful? %d\n", argc); // this does actually accept each "word" with a whitespace as arg + 1 
	int i;
	for(i = 1; i < argc; i++){ // because 0 is the program call
		printf("%dth arg: %s\n",i, *(argv + i));
	}
	// i think like in a loop here you could check whether the newly added node's op is -1 or not
	// then call for pop and pop should do the popping 3 times thing on its own
	// PLEASE NO RECURSION THOUGH i care about stack overflows thank you
}

// if this is called then we must have hit on an operator
int pop(Element **headPtr){ // we need to pop 3 times!!
	int operands[2] = {0};
	enum operator this = 0;
	double final_val = 0;
	char *val_str = NULL;
	// remove the last node and return the num of it
	if(*headPtr == NULL){
		puts("empty stack can't be popped!");
		return -1;
	}
	else if((*headPtr)->is_known && (*headPtr)->op != -1){
		// the top is basically the operand we need to use
		this = (*headPtr)->op;
		if((*headPtr)->bottom->is_known && (*headPtr)->bottom->value != -1){
			// the second from the top is an operand!
			operands[0] = (*headPtr)->bottom->value;
			if((*headPtr)->bottom->bottom->is_known && (*headPtr)->bottom->bottom->value != -1){
				// third from the top also is an operand so do calculations here congrats!!!!
				operands[1] = (*headPtr)->bottom->bottom->value;
				switch(this){
				case MULTIPLICATION: final_val = operands[0] * operands[1]; break;
				case ADDITION: final_val = operands[0] + operands[1]; break;
				case SUBTRACTION: final_val = operands[0] - operands[1]; break;
				case DIVISION: final_val = operands[0] / operands[1]; break;
				case EXPONENT: final_val = pow(operands[0], operands[1]); break;
				default: puts("look into line 53");
				}
				// pop 3 elements then call push with final_val
				*headPtr = (*headPtr)->bottom->bottom->bottom;
				snprintf(val_str, 20, "%d", final_val);
				push(headPtr, val_str);
			}
		}
	}
	// pop all stuff, calculate and then push the new entry

	//int t = (*headPtr)->num;
	//*headPtr = (*headPtr)->bottom;
	return 0;
	/*else if((*headPtr)->bottom == NULL){
		puts("removing the last item");
		int t = (*headPtr)->num;
		*headPtr = NULL;
		return t;
	}*/
}

void push(Element **headPtr, char data){
	//creation of the node

	Element *newNode = malloc(sizeof(struct element));
	if(isdigit(data)){
		newNode->value = data - '0';
		newNode->is_known = 1;
		newNode->op = -1;
	}
	else if(data == '-' || data == '+' || data == '*' || data == '/' || data == '^'){
		newNode->value = -1;
		newNode->op = data;
		newNode->is_known = 1;
	}
	else { // a variable
		newNode->value = -1;
		newNode->op = -1;
		newNode->is_known = -1;
	}
	newNode->bottom = NULL;

	// placement of the newly created node
	if(*headPtr == NULL){
		*headPtr = newNode;
	}
	else{
		newNode->bottom = *headPtr;
		*headPtr = newNode;
	}
}