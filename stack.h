/*
    Implementation of stack for expression processing

*/

#ifndef STACK_H
#define STACK_H


#include <stdbool.h>
#include <stdlib.h>

#include "scanner.h"
#include "errorslist.h" 

#define MAX_STACK 50

/**
 * Stack structure
 */
typedef struct {
    
    Data_type data[MAX_STACK];
    
    int top;

} Stack;

void s_init ( Stack *stack );

/**
 * Pushes item into stack
 * @param stack stack for expression processing
 * @param data data type
 */
void s_push ( Stack *stack, Data_type data );




bool s_is_full(Stack *stack);

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
Data_type s_top ( Stack *stack );

/**
 * Discards all remaining items
 */ 
void s_dispose ( Stack *stack );

bool s_is_empty(Stck *stack);



#endif
