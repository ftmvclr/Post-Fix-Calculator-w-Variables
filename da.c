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

struct element{
	double value; // the value if it is an operand, -1 if not 
	char op; // the char if it is an operator, -1 if not an operator (EOF)
	int is_known; // 0 means variable (letter), 1 means "value or op" is not -1
	char var_name; // -1 if not a variable
	struct element *bottom; // for the stack
	int currently_what;
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
void recursive_test(VarPtrs *current_var, Element *headPtr);
double pop_but_not_brutal(Element *headPtr);

int var_count = 0;
int option_count = 0;

// pointer to char array
int main(int argc, char *argv[]){
	equalityCheck = (Element *)malloc(sizeof(struct element)); // feels redundant but don't have a better alt
	if(equalityCheck == NULL) return -1;
	equalityCheck->value = -1;
	equalityCheck->op = EQUALS;
	equalityCheck->is_known = 1; // technically
	equalityCheck->var_name = '=';
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
	option_count = (int)(pow(2, var_count) + 0.5); // this redundancy is to prevent it from miscalculating
	// we could strategize better? do i have to do it recursively??? noooooooooooo
	stack_printer(headPtr);
	stack_reverser(&headPtr);
	stack_printer(headPtr);
	// could loop honestly cant i
	int varnum; // first variable, we gotta iterate with this too until it is equal to var_count
	for(i = 1; i <= option_count; i++){ // which option we are at
		VarPtrs *current = varptr_starter;

		for(varnum = 1; varnum <= var_count; varnum++){ // which variable we are at
			// is an operator IF: floor(i/(option_count / 2*varnum))% 2 <= 1
			if(current == NULL)
				break;
			if((int)floor(i / (option_count / pow(2, varnum))) % 2 == 1){ // why does floor even return float??????
				// test varnum being an operator!!!
				current->this->currently_what = 1;
				current->this->op = MULTIPLICATION; // a placeholder fornow
				current->this->value = -1;
			}
			else{
				// test varnum being a positive integer instead in whatever # option we are at
				current->this->currently_what = 0;
				current->this->op = -1;
				current->this->value = 1; // so that it counts as a valid value FOR NOW (placeholder)
			}
			current = current->next;
		}
		printf("Option %d: ", i);
		VarPtrs *traversing_options = varptr_starter;
		while(traversing_options){ // while it is not null
			printf("%c=%s ", traversing_options->this->var_name,
				(traversing_options->this->currently_what == 1 ? "Operator" : "Operand"));
			traversing_options = traversing_options->next;
		}
		printf("\n");

		recursive_test(varptr_starter, headPtr);
		// we need some logic to save the true ones in like an array so that we can print them right?
		// NO need for saving just print the moment you find the right option with right values.
	}

}
// at this point all of our variables were artificially updated 
// ALL YOU HAVE TO DO IS GO THROUGH THE STACK AND POP THEM NODES in a loop
void recursive_test(VarPtrs *current_var, Element *headPtr){
	if(current_var == NULL) { // base case: end of list
		double result = pop_but_not_brutal(headPtr);

		if((result == 0 || fabs(result) < 0.00001) && !isnan(result)){ // numerical methods paranoia
			printf("(");
			int i = 0;
			VarPtrs *printer = varptr_starter;

			while(printer != NULL) {
				if(printer->this->currently_what == 1){ // operator
					printf("%c", printer->this->op);
				}
				else { // num
					printf("%.0f", printer->this->value); // .0f so it formats like int
				}
				if(printer->next != NULL) // , unless it is the end
					printf(", ");

				printer = printer->next;
			}
			printf(")\n");
		}
		return; // this was basically one option's one solution there MIGHT be more
	}

	if(current_var->this->currently_what == 1) { // operator
		char ops[] = {'*', '+', '-', '/', '^'};
		int i;
		for(i = 0; i < 5; i++) {
			current_var->this->op = ops[i];
			recursive_test(current_var->next, headPtr);
		}
	}

	else { // so not one (0), a number
		int i;
		for(i = 1; i <= 20; i++) {
			current_var->this->value = i * 1.0;
			recursive_test(current_var->next, headPtr);
		}
	}
}
// the other pop function decimates the stack
// instead of reconstructing the stack from top to bottom as LL 
// let's just use an array and decide if it is a valid option 
// whilst building it
double pop_but_not_brutal(Element *headPtr){
	double temp_stack[100] = {0};
	int top = -1; // this is the condition for an empty stack
	Element *current = headPtr;

	while(current != NULL) {
		// this includes both the actual numbers and the variables being checked as an int right now
		if((current->is_known && current->value != -1) ||
			(!current->is_known && current->currently_what == 0)){
			double will_push_this = current->value; // variable value (testing this) OR the actual thing, doesnt matter
			temp_stack[++top] = will_push_this;
		}

		// this part is for if it is an operator
		else {
			// success!! you reached equal sign now see what the top of the stack is 
			if(current->is_known && current->op == EQUALS){
				if(top == 0 && fabs(temp_stack[0]) < 0.00001) // top being zero has to be checked because the first input could simply be zero
					return 0;
				return NAN; // Otherwise fail
			}
			// empty?
			if(top < 1)
				return NAN;

			double right_operand = temp_stack[top--];
			double left_operand = temp_stack[top--];
			double result = 0;

			char op_char = current->op; // this var's op, or OG op, again doesn't matter necessarily

			switch(op_char) {
			case MULTIPLICATION: result = left_operand * right_operand; break;
			case ADDITION: result = left_operand + right_operand; break;
			case SUBTRACTION: result = left_operand - right_operand; break;
			case DIVISION:
				if(right_operand == 0)
					return NAN; // dividing by zero is unhealthy for the compiler 
				result = left_operand / right_operand;
				break;
			case EXPONENT: result = pow(left_operand, right_operand); break;
			default: return NAN;
			}
			// is the result an integer?
			if(result - (int)result != 0){
				return NAN;
			}
			/*if(floor(result) != result){
				return NAN;
			}*/
			temp_stack[++top] = result; // the result is pushed instead to the array-stack
		}
		current = current->bottom;
	}

	if(top == 0)
		return temp_stack[0];
	return NAN; // for fail
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
	//	stack_printer(newHeadPtr);
	*headPtr = newHeadPtr;
	return newHeadPtr;
}

void addToVarsLL(Element *node){
	VarPtrs *temp = (VarPtrs *)malloc(sizeof(VarPtrs));
	static VarPtrs *tail = NULL;
	if(temp == NULL) return;
	temp->next = NULL;
	temp->this = node;
	if(varptr_starter == NULL && tail == NULL){
		// so first node 
		varptr_starter = temp;
		tail = temp;
	}
	else{
		// not the first node
		tail->next = temp;
		tail = temp;
	}
}