/*
    Implementation of stack for expression processing

*/


#include "stack.h"

void s_init ( Stack *stack ) {

    stack->top = NULL;

}

void s_push ( Stack *stack, Data_type data ) {

    Stack_item *newItem = malloc( sizeof ( Stack_item ) );
    if (!newItem) return false;

    newItem->data = data;
    newItem->nextItem = stack->top;
    stack->top = newItem;
    
}

void s_push_after_terminal ( Stack *stack, Data_type data ) {

    return true;

}

void s_pop ( Stack *stack ) {

    Stack_item *toPop = stack->top;
    stack->top = stack->top->nextItem;
    free( toPop );

}

Stack_item *s_top ( Stack *stack ) {

    return stack->top;

}

void s_dispose ( Stack *stack ) {

    while (stack->top) {

        s_pop( stack );       

    }

}