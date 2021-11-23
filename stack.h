/*
    Implementation of stack for expression processing

*/

#ifndef STACK_H
#define STACK_H

#include "scanner.h"
#include "errorslist.h" 
#include "expression.h"

#include <stdbool.h>
#include <stdlib.h>


/**
 * Stack structure
 */
typedef struct SElem {

    pt_terminal symbol;
    Data_type type;
    struct SElem *nextItem;

} Stack_item;

typedef struct {
    
    Stack_item *top;

} Stack;

void s_init ( Stack *stack );

/**
 * Pushes item into stack
 * @param stack stack for expression processing
 * @param data data type
 */
void s_push ( Stack *stack, Data_type data, pt_terminal symbol );

/**
 * Pushes item into stack before first occured terminal (only for shift)
 */
void s_push_before_terminal ( Stack *stack, Data_type data, pt_terminal symbol );

/**
 * Discard one stack item
 */
void s_pop ( Stack *stack );

/**
 * Returns stack's top item
 */
Stack_item *s_top ( Stack *stack );

Data_type s_top_type ( Stack *stack );

/**
 * Returns first terminal
 */
int s_top_terminal_symbol ( Stack *stack );

/**
 * Discards all remaining items
 */ 
void s_dispose ( Stack *stack );   


void s_dispose_and_print ( Stack *stack );



#endif