/*
    Implementation of stack for expression processing

*/

#ifndef STACK_H
#define STACK_H


#include <stdbool.h>
#include <stdlib.h>

#include "scanner.h"
#include "errorslist.h" 

/**
 * Stack structure
 */
typedef struct SElem {

    Data_type data;
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
void s_push ( Stack *stack, Data_type data );

/**
 * 
 */
void s_push_after_terminal ( Stack *stack, Data_type data );

/**
 * Discard one stack item
 */
void s_pop ( Stack *stack );

/**
 * Returns stack's top item
 */
Stack_item *s_top ( Stack *stack );

/**
 * Discards all remaining items
 */ 
void s_dispose ( Stack *stack );   



#endif