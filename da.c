#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

// integer check could be like subtracting original number from its redefinition with int?
// like a = 4.3 then we just go for a new initialization which is:
// int b = a; then this will be 4 and we can substract a - b == 0?
// even ternary op would fit just fine here and there 

// also what if we kept the pointers to these variable nodes in some array, might be easier to adjust and then test?
// ye just update them and go for solve stack or something? i think we can implement one more function for just that
// we now have a whole linked list dedicated to holding variables we can traverse them and like in a function
// we could send some array with values like 1,0s which indicate if they are op or num
// then in that function what do we test though 

enum operator {MULTIPLICATION = 42, ADDITION, SUBTRACTION = 45, DIVISION = 47, EQUALS = 61, EXPONENT = 94};

struct element{ // 
	double value; // the value if it is an operand, -1 if not 
	char op; // the char if it is an operator, -1 if not an operator (EOF)
	int is_known; // 0 means variable (letter), 1 means "value or op" is not -1
	char var_name; // -1 if not a variable
	struct element *bottom; // for the stack
};
typedef struct element Element;
Element *equalityCheck;

struct varPtrs{ // linked list
	struct varPtrs *next; // linker
	Element *this; // datatype 
} *varptr_starter; // should be null initially
typedef struct varPtrs VarPtrs;

Element *push(Element **headPtr, char *dataS);
int pop(Element **headPtr);
void stack_printer(Element *headPtr);
Element *stack_reverser(Element **headPtr);
void addToVarsLL(Element *node);

int var_count = 0;
int option_count = 0;

// pointer to char array
int main(int argc, char *argv[]){
	equalityCheck = (Element *)malloc(sizeof(struct element)); // feels redundant but don't have a better alt
	if(equalityCheck == NULL) return -1;
	equalityCheck->value = -1;
	equalityCheck->op = -1;
	equalityCheck->is_known = 1; // technically
	equalityCheck->var_name = -1;
	equalityCheck->bottom = NULL;

	Element *headPtr = NULL;
	Element *tempNode = NULL;

	printf("successful? %d\n", argc - 1); // this does actually accept each "word" with a whitespace as arg + 1 
	int i;
	for(i = 1; i < argc; i++){ // because 0 is the program call
		printf("%dth arg: %s\n", i, *(argv + i));
		tempNode = push(&headPtr, *(argv + i));
		if(tempNode == NULL || tempNode == equalityCheck) {
			puts("malloc failed? OR we hit \"=\"\n");
			break;
		}
		if(tempNode->op != -1){ // so + - / etc.
			puts("push success operator");
			pop(&headPtr);
		}
		else if(tempNode->value != -1){
			puts("push success operand");
		}
		else if(tempNode->is_known == 0){
			puts("pushed a variable");
			var_count++; // should work.
		}
	}// traversed the input and simplified the expression at this point
	// now what?
	// let's have the count of variables i think if we knew about that
	option_count = (int)pow(2, var_count); // n
	// we could strategize better? do i have to do it recursively??? noooooooooooo
	stack_printer(headPtr);
	stack_reverser(&headPtr);
	// could loop honestly cant i
	int varnum = 1; // first variable, we gotta iterate with this too until it is equal to var_count
	for(i = 1; i <= option_count; i++){ // which option we are at
		for(; varnum <= var_count; varnum++){ // which variable we are at
			// is an operator IF: floor(i/(option_count / 2*varnum))% 2 <= 1
			if((int)floor(i / (option_count / 2.0 * varnum)) % 2 <= 1){ // why does floor even return float??????
				// test varnum being an operator!!!
			}
			else{
				// test varnum being a positive integer instead in whatever # option we are at
			}
		}
		// we need some logic to save the true ones in like an array so that we can print them right?
	}
}

// if this is called then we must have hit on an operator
int pop(Element **headPtr){ // we need to pop 3 times!!
	double operands[2] = {0};
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
			operands[1] = (*headPtr)->bottom->value;
			if((*headPtr)->bottom->bottom->is_known && (*headPtr)->bottom->bottom->value != -1){
				// third from the top also is an operand so do calculations here congrats!!!!
				operands[0] = (*headPtr)->bottom->bottom->value;
				switch(this){
				case MULTIPLICATION: final_val = operands[0] * operands[1]; break;
				case ADDITION: final_val = operands[0] + operands[1]; break;
				case SUBTRACTION: final_val = operands[0] - operands[1]; break;
				case DIVISION: final_val = operands[0] / operands[1]; break;
				case EXPONENT: final_val = pow(operands[0], operands[1]); break; // check if this is the order
				default: puts("look into line 53");
				}
				// pop 3 elements then just add the result here honestly
				*headPtr = (*headPtr)->bottom->bottom->bottom;
				Element *newNode = (Element *)malloc(sizeof(struct element));
				if(newNode == NULL) return -1; // run
				newNode->value = final_val;
				newNode->bottom = NULL;
				newNode->op = -1;
				newNode->is_known = 1; // yes!
				if(*headPtr == NULL && newNode != NULL){
					*headPtr = newNode;
				}
				else if(newNode != NULL){
					newNode->bottom = *headPtr;
					*headPtr = newNode;
				}
				printf("we popped 3 elements and inserted this instead:"\
					"%f, due to the operation %c: \n", final_val, this);
			}
		}
	}
	return 0;
}

Element *push(Element **headPtr, char *dataS){
	//creation of the node

	Element *newNode = (Element *)malloc(sizeof(struct element));
	if(newNode == NULL) {
		return NULL;
	}

	// Check if the FIRST char of the string is a digit
	if(isdigit((unsigned char)dataS[0])) {
		// Use strtol to convert the WHOLE string ("62" -> 62)
		newNode->value = strtol(dataS, NULL, 10);
		newNode->is_known = 1;
		newNode->op = -1;
	}
	// Check if it's a 1-char-long string
	else if(strlen(dataS) == 1) {
		char data = dataS[0];
		if(data == '-' || data == '+' || data == '*' || data == '/' || data == '^') {
			newNode->value = -1;
			newNode->op = data;
			newNode->is_known = 1;
			newNode->var_name = -1;
		}
		else if(data == '='){
			newNode = equalityCheck;
		}
		else { // a variable
			newNode->value = -1;
			newNode->op = -1;
			newNode->is_known = 0;
			newNode->var_name = data;
			addToVarsLL(newNode);
		}
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
	return newNode;
}

void stack_printer(Element *headPtr){
	while(headPtr){ // when it is not null basically
		if(headPtr->is_known)
			if(headPtr->value != -1)
				printf("%f ", headPtr->value);
			else
				printf("%c ", headPtr->op);
		else
			printf("%c ", headPtr->var_name);
		headPtr = headPtr->bottom;
	}
	puts("");
}

Element *stack_reverser(Element **headPtr){
	Element *newHeadPtr = NULL;
	Element *previous = NULL;
	Element *current = *headPtr;
	Element *next = NULL;

	while(current){
		next = current->bottom;
		current->bottom = previous;
		previous = current;
		current = next;
	}
	newHeadPtr = previous;
	stack_printer(newHeadPtr);
	return newHeadPtr;
}

void addToVarsLL(Element *node){
	VarPtrs *temp = (VarPtrs *)malloc(sizeof(VarPtrs));
	if(temp == NULL) return;
	temp->next = NULL;
	temp->this = node;
	if(varptr_starter == NULL){
		// so first node 
		varptr_starter = temp;
	}
	else{
		// not the first node
		temp->next = varptr_starter;
		varptr_starter = temp;
	}
}