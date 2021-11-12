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

    return true;//what is it???

}

void s_pop ( Stack *stack ) {

    if(s_is_empty(stack){
        //call error code
        return;
    }else{
        stack->top--;

}

Data_type *s_top ( Stack *stack ) {

    if(s_is_empty(stack){
        //call error code
        return;
    }
       
    return stack->data[stack->top];

}

void s_dispose ( Stack *stack ) {

    free(stack);

}
