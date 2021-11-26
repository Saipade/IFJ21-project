/**
 * PROJECT:
 */


#include "stack.h"

void s_init ( Stack *stack ) {

    stack->top = NULL;

}

void s_push ( Stack *stack, Data_type type, pt_symbol symbol ) {

    Stack_item *newItem = malloc( sizeof ( Stack_item ) );
    if (!newItem) return;

    newItem->nextItem = stack->top;
    newItem->symbol = symbol;
    newItem->type = type;
    stack->top = newItem;
    
}

void s_push_before_terminal ( Stack *stack, Data_type type, pt_symbol symbol ) {

    Stack_item *newItem = malloc( sizeof ( Stack_item ) );
    if (!newItem) return;
    Stack_item *tmp1 = stack->top;
    Stack_item *tmp2 = NULL;
    newItem->symbol = symbol;
    newItem->type = type;

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

Data_type s_top_type ( Stack *stack ) {

    return stack->top->type;

}

int s_top_terminal_symbol ( Stack *stack ) {

    Stack_item *topTerminal = stack->top;
    
    while (topTerminal) {

        if (topTerminal->symbol != NONTERM && topTerminal->symbol != STOP) break;
        topTerminal = topTerminal->nextItem;

    }

    return topTerminal->symbol;

}

void s_dispose ( Stack *stack ) {

    while (stack->top) {
        
        s_pop( stack );       

    }

}
// for debugging reasons
void s_dispose_and_print ( Stack *stack ) {
    printf("(symbol type)");
    while (stack->top) {
        printf("(%d %d)", stack->top->symbol, stack->top->type);
        s_pop( stack );       

    }

}