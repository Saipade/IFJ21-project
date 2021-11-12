/*
    Implementation of stack for expression processing

*/


#include "stack.h"

void s_init ( Stack *stack ) {

    if(stack == NULL){
        //call error 
        return;
    }
    stack->top = -1;

}

void s_push ( Stack *stack, Data_type data ) {

    if(s_is_full(stack)){
        //call error code
        return;
    }
    stack->top++;
    stack->data[stack->top] = data;
    
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
