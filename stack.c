/*
    Implementation of stack for expression processing

*/


#include "stack.h"

void s_init ( Stack *stack ) {

    stack->top = NULL;

}

void s_push ( Stack *stack, Data_type data, pt_terminal symbol ) {

    Stack_item *newItem = malloc( sizeof ( Stack_item ) );
    if (!newItem) return;

    newItem->nextItem = stack->top;
    stack->top = newItem;
    
}

void s_push_before_terminal ( Stack *stack, Data_type data, pt_terminal symbol ) {

    Stack_item *newItem = malloc( sizeof ( Stack_item ) );
    if (!newItem) return;
    Stack_item *tmp1 = stack->top;
    Stack_item *tmp2;
    newItem->symbol = symbol;

    while (1) {

        if (!tmp1) break;

        if (tmp1->symbol != NONTERM && tmp1->symbol != STOP) {
            
            if (!tmp2) {
                newItem->nextItem = stack->top;
                stack->top = newItem;
            }

            else {
                newItem->nextItem = tmp1;
                tmp2->nextItem = newItem;
            }

            return;

        }

        tmp2 = tmp1;
        tmp1 = tmp1->nextItem;

    }

}

void s_pop ( Stack *stack ) {

    Stack_item *toPop = stack->top;
    stack->top = stack->top->nextItem;
    free( toPop );

}

Stack_item *s_top ( Stack *stack ) {

    return stack->top;

}

Stack_item *s_top_terminal ( Stack *stack ) {

    Stack_item *topTerminal = stack->top;
    
    while (1) {

        if (topTerminal->symbol != NONTERM && topTerminal->symbol != STOP) return topTerminal;
        topTerminal = topTerminal->nextItem;

    }

}

void s_dispose ( Stack *stack ) {

    while (stack->top) {

        s_pop( stack );       

    }

}