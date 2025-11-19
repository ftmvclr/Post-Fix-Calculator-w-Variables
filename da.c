#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

enum operator {MULTIPLICATION = 42, ADDITION, SUBTRACTION = 45, DIVISION = 47, EQUALS = 61, EXPONENT = 94};
struct element{
	double value; // the value if it is an operand, -1 if not 
	char op; // the char if it is an operator, -1 if not an operator (EOF)
	int is_known; // 0 means variable (letter), 1 means "value or op" is not -1
	struct element *bottom; // for the stack
};
typedef struct element Element;

Element *push(Element **headPtr, char *dataS);
int pop(Element **headPtr);
Element *equalityCheck;
int var_count = 0;

// pointer to char array
int main(int argc, char *argv[]){
	equalityCheck = (Element *)malloc(sizeof(struct element));
	if(equalityCheck == NULL) return -1;

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
	printf("LOOK here \n%d", var_count);
	// now what?
	// let's have the count of variables i think if we knew about that
	// we could strategize better? do i have to do it recursively??? noooooooooooo
	// could loop honestly cant i

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
		}
		else if(data == '='){
			newNode = equalityCheck;
		}
		else { // a variable
			newNode->value = -1;
			newNode->op = -1;
			newNode->is_known = 0;
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