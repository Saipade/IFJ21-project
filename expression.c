/**
 * 
 */

#include "errorslist.h"
#include "scanner.h"
#include "expression.h"
#include "stack.h"
#include "code_generator.h"

#include <stdbool.h>
#include <stdlib.h>
// symbol on top has higher priority than next symbol -> shift
// symbol on top has lower or same priority compared to the next symbol -> reduce
// S - shift; R - reduction; E - equal; B - blank (not defined rule)
int precTable[9][9] = {
//input|+- |*/ |cmp| ( | ) | i | # |.. | $    // top of stack
	   { R , S , R , S , R , S , S , R , R }, // +-
	   { R , R , R , S , R , S , S , R , R }, // * / //
	   { S , S , B , S , R , S , S , S , R }, // cmp
	   { S , S , S , S , E , S , S , S , B }, // (
	   { R , R , R , B , R , B , B , R , R }, // )
	   { R , R , R , B , R , B , R , R , R }, // i
       { R , R , R , B , R , S , B , B , R }, // #
       { S , S , R , S , R , S , B , S , R }, // ..
	   { S , S , S , S , B , S , S , S , B }, // $
};

Stack st;
Stack *stack = &st;

pt_terminal get_op ( Token *token ) {

    switch (token->type) {

        case T_ADD:
        case T_SUB:
            return I_PLUSMINUS;

        case T_MUL:
        case T_DIV:
        case T_IDI:
            return I_MULDIV;
            
        case T_LTH:
        case T_LET:
        case T_MTH:
        case T_MET:
        case T_IEQ:
        case T_NEQ:
            return I_CMP;

        case T_LBR:
            return I_LBR;
            
        case T_RBR:
            return I_RBR;

        case T_INT:
        case T_NUM:
        case T_STR:
        case T_NIL:
        case T_IDE:
            return I_i;

        case T_LEN:
            return I_HASH;

        case T_CAT:
            return I_CAT;

        default:
            return I_DOLLAR;

    }
}

int get_pt_index ( pt_terminal symbol ) {

    switch (symbol) {
        

    }

}

int expression ( Parser_data *parserData ) {

    int res;
    int symbol;
    bool success = 0;

    s_init( stack );
    s_push( stack, DOL );


}

int _shift ( Parser_data *parserData ) {

    int res;


    return 0;
    
}

int _reduce ( Parser_data *parserData ) {

    int res;
    int count = 0;
    bool found = 0;

    Stack_item *item1 = NULL;
    Stack_item *item2 = NULL;
    Stack_item *item3 = NULL;
    Stack_item *tmp = s_top( stack );

    Data_type finalType;
    pt_rule ruleName;

    while (1) {
        
        if (tmp == NULL) break;

        if (tmp->symbol == STOP) {
            found = 1;
            break;
        }

        else count++;

        tmp = tmp->nextItem;

    }

    if (found == 0) return ERR_SYNTAX;

    if      (count == 1) {
        item3 = stack->top;
        ruleName = check_rule( count, NULL, NULL, item3 );
    }

    else if (count == 2) {
        item3 = stack->top->nextItem;
        item2 = stack->top;
        ruleName = check_rule( count, NULL, item2, item3 );
    }

    else if (count == 3) {
        item3 = stack->top->nextItem->nextItem;
        item2 = stack->top->nextItem;
        item1 = stack->top;
        ruleName = check_rule( count, item1, item2, item3 );
    }

    
    


    return 0;

}

/**
 * Function checks if there is relevant rule for items 1-3
 * @param count number of stack items before STOP sign 
 * @param item1 next after next after top
 * @param item2 next after top 
 * @param item3 top of the stack
 * @return rulename (enum)
 */
pt_rule check_rule ( int count, Stack_item *item1, Stack_item *item2, Stack_item *item3 ) {

    switch (count) {
        
        case 1:
            // E -> i
            if (IS_I( item3 )) {
                return NT_RULE;
            }

            return ND_RULE;

        break;

        case 2:
            // E -> #E
            if (item2->symbol == LEN && item3->symbol == NONTERM) {
                return LEN_NT;
            }

            return ND_RULE;

        break;

        case 3:
            // E -> E operator E
            if (item1->symbol == NONTERM && item3->symbol == NONTERM)
                // E -> E + E
                     if (item2->symbol == ADD) return NT_PLUS_NT;
                // E -> E - E
                else if (item2->symbol == SUB) return NT_MINUS_NT;
                // E -> E * E
                else if (item2->symbol == MUL) return NT_MUL_NT;
                // E -> E / E
                else if (item2->symbol == DIV) return NT_DIV_NT;
                // E -> E // E
                else if (item2->symbol == IDI) return NT_IDIV_NT;
                // E -> E .. E
                else if (item2->symbol == CAT) return NT_CAT_NT;
                // E -> E < E
                else if (item2->symbol == LTH) return NT_LTH_NT;
                // E -> E <= E
                else if (item2->symbol == LET) return NT_LET_NT;
                // E -> E > E 
                else if (item2->symbol == MTH) return NT_MTH_NT;
                // E -> E >= E
                else if (item2->symbol == MET) return NT_MET_NT;
                // E -> E == E
                else if (item2->symbol == EQU) return NT_IEQ_NT;
                // E -> E ~= E
                else if (item2->symbol == NEQ) return NT_NEQ_NT;

                else return ND_RULE;
            
            else {
            
                if (item1->symbol == LBR && item3->symbol == RBR && item2->symbol == NONTERM) return LBR_NT_RBR;

            }

    }           


}