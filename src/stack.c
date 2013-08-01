#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct stackstructor {
	type elem[MAX_SIZE];
	int top;
}stack;

void init_stack(stack *original_stack)
{
	original_stack->top = -1;
}

int is_stack_empty(stack original_stack)
{
	if (original_stack.top == -1)
		return 0;
	else
		return 1;
}

int push(stack *original_stack, type varible)
{
	if (original_stack->top == MAX_SIZE - 1) {
		printf("The stack is oready full!\n");
		return 1;
	}
	original_stack->top++;
	original_stack->elem[original_stack->top] = varible;

	return 0;
}

int pop(stack *original_stack, type *varible)
{
	if ((is_stack_empty(*original_stack)) == 0) {
		printf("No element to pop!\n");
	return 1;
	}
	*varible = original_stack->elem[original_stack->top];
	original_stack->top--;

	return 0;
}

int get_top_stack(stack *original_stack, type *varible)
{
	if (is_stack_empty(*original_stack) == 0) {
		printf("No element to get!\n");
		return 1;
	}
	*varible = original_stack->elem[original_stack->top];

	return 0;
}
#endif
