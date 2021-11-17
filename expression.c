/**
 *  PROJECT: Implementation of imperative language translator (IFJ21)
 *  PART: EXPRESSION PROCESSING
 * AUTHOR(S): Maksim Tikhonov (xtikho00)
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
pt_operation precedenceTable[9][9] = {
//input|+- |*/ |rel| ( | ) | i | # |.. | $    // top of stack
	   { R , S , R , S , R , S , S , R , R }, // +-
	   { R , R , R , S , R , S , S , R , R }, // * / //
	   { S , S , B , S , R , S , S , S , R }, // rel
	   { S , S , S , S , E , S , S , S , B }, // (
	   { R , R , R , B , R , B , B , R , R }, // )
	   { R , R , R , B , R , B , R , R , R }, // i
       { R , R , R , S , R , S , B , R , R }, // # // (.. x #) = ????
       { S , S , R , S , R , S , B , S , R }, // ..
	   { S , S , S , S , B , S , S , S , B }, // $
};

int rule_Expression ( Parser_data *parserData );
int shift ( Parser_data *parserData, Stack *stack, pt_terminal symbol );
int reduce ( Parser_data *parserData );
pt_rule check_rule ( int count, Stack_item *item1, Stack_item *item2, Stack_item *item3 );
int test_semantic ( Stack_item *item1, Stack_item *item2, Stack_item *item3, pt_rule rule );
pt_terminal convert_token_to_symbol ( Token *token );
pt_index get_pt_index ( pt_terminal symbol );
int save_result ( Parser_data *parserData, Stack *stack );

Stack st;
Stack *stack = &st;

pt_terminal convert_token_to_symbol ( Token *token ) {

    switch (token->type) {

        case T_INT:
        return INT;
        case T_NUM:
        return NUM;
        case T_STR:
        return STR;
        case T_BOO:
        return BOO;
        case T_NIL:
        return NIL;
        case T_IDE:
        return IDE;
        case T_ADD:
        return ADD;
        case T_SUB:
        return SUB;
        case T_MUL:
        return MUL;
        case T_DIV:
        return DIV;
        case T_IDI:
        return IDI;
        case T_CAT:
        return CAT;
        case T_LEN:
        return LEN;
        case T_LTH:
        return LTH;
        case T_LET:
        return LET;
        case T_MTH:
        return MTH;
        case T_MET:
        return MET;
        case T_EQU:
        return EQU;
        case T_NEQ:
        return NEQ;
        case T_LBR:
        return LBR;
        case T_RBR:
        return RBR;
        default:
        return DOL;

    }

}

pt_index get_pt_index ( pt_terminal symbol ) {

    switch (symbol) {

        case ADD:
        case SUB:
            
        return I_PLUSMINUS;

        case MUL:
        case DIV:
        case IDI:
            
        return I_MULDIV;
            
        case LTH:
        case LET:
        case MTH:
        case MET:
        case EQU:
        case NEQ:
            
        return I_CMP;

        case LBR:
            
        return I_LBR;
            
        case RBR:
        
        return I_RBR;

        case INT:
        case NUM:
        case STR:
        case NIL:
        case IDE:
        
        return I_i;

        case LEN:
        
        return I_LEN;

        case CAT:
        
        return I_CAT;

        default:
        
        return I_DOLLAR;

    }
}


int rule_Expression ( Parser_data *parserData ) {

    int res = 0;

    s_init( stack );
    s_push( stack, T_DOL, DOL );
    
    Stack_item *firstTerminal;
    pt_terminal nextSymbol;
    pt_operation currentOperation;
    bool success = false;

    while (1) {
        
        firstTerminal = s_top_terminal( stack );
        nextSymbol = convert_token_to_symbol( &parserData->token );
        if (nextSymbol == DOL) exit( ERR_SYNTAX );
        currentOperation = precedenceTable[get_pt_index(firstTerminal->symbol)][get_pt_index(nextSymbol)];
        switch (currentOperation) {
            // Shift
            case S:

                if (res = shift( parserData, stack, nextSymbol )) return res;            
                if (res = get_next_token( &parserData->token )) return res;

            break;
            // Reduce
            case R:

                if (res = reduce( parserData )) return res;
                if (res = get_next_token( &parserData->token )) return res;

            break;
            // Equal
            case E:
                
                s_push( stack, parserData->token.type, nextSymbol );
                if (res = get_next_token( &parserData->token )) return res;

            break;
            // Blank space
            case B:

                if (nextSymbol == DOL && firstTerminal->symbol == DOL) {
                    success = 1;
                    break;
                }
                else exit( ERR_SYNTAX );

            break;

        }

    }

    if (res = save_result( parserData, stack ));

    s_dispose( stack );

    return res;

}

int shift ( Parser_data *parserData, Stack *stack, pt_terminal symbol ) {

    int res = 0;

    s_push_before_terminal( stack, T_NDA, STOP );
    s_push( stack, parserData->tokenTable->type, symbol );
    if (IS_I(symbol)) 
    cg_push( &parserData->token );
    
    return 0;
    
}
/**
 * Applies one of the rules up to 3 symbols from stack
 * 
 * 
 */
int reduce ( Parser_data *parserData ) {

    int res = 0;
    int count = 0;
    bool found = 0;

    Stack_item *item1 = NULL;
    Stack_item *item2 = NULL;
    Stack_item *item3 = NULL;
    Stack_item *tmp = s_top( stack );

    pt_rule ruleName;
    // count symbols before stop sign
    while (true) {
        
        if (tmp == NULL) break;

        if (tmp->symbol == STOP) {
            found = true;
            break;
        }

        else count++;

        tmp = tmp->nextItem;

    }

    if (found == 0) exit( ERR_SYNTAX );

    if      (count == 1) {
        item1 = stack->top;
        item2 = NULL;
        item3 = NULL;
        ruleName = check_rule( count, item1, item2, item3 );
    }

    else if (count == 2) {
        item1 = stack->top;
        item2 = stack->top->nextItem;
        item3 = NULL;
        ruleName = check_rule( count, item1, item2, item3 );
    }

    else if (count == 3) {
        item1 = stack->top;
        item2 = stack->top->nextItem;
        item3 = stack->top->nextItem->nextItem;
        ruleName = check_rule( count, item1, item2, item3 );
    }

    if (ruleName == ND_RULE) exit( ERR_SYNTAX );

    Data_type resType = test_semantic( item1, item2, item3, ruleName );

    // pop out all "consumed" symbols and stop sign
    for (int i = 0; i <= count; i++) s_pop( stack );
    // and push created non-terminal
    s_push( stack, resType, NONTERM );

    return res;

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
            if (IS_I( item1->symbol )) {
                return E_RULE;
            }

        return ND_RULE;

        case 2:

            // E -> #E
            if (item1->symbol == LEN && item2->symbol == NONTERM) {
                return LEN_E;
            }

        return ND_RULE;

        case 3:

            // E -> E operator E
            if (item1->symbol == NONTERM && item3->symbol == NONTERM)
                // E -> E + E
                     if (item2->symbol == ADD) return E_PLUS_E;
                // E -> E - E
                else if (item2->symbol == SUB) return E_MINUS_E;
                // E -> E * E
                else if (item2->symbol == MUL) return E_MUL_E;
                // E -> E / E
                else if (item2->symbol == DIV) return E_DIV_E;
                // E -> E // E
                else if (item2->symbol == IDI) return E_IDIV_E;
                // E -> E .. E
                else if (item2->symbol == CAT) return E_CAT_E;
                // E -> E < E
                else if (item2->symbol == LTH) return E_LTH_E;
                // E -> E <= E
                else if (item2->symbol == LET) return E_LET_E;
                // E -> E > E 
                else if (item2->symbol == MTH) return E_MTH_E;
                // E -> E >= E
                else if (item2->symbol == MET) return E_MET_E;
                // E -> E == E
                else if (item2->symbol == EQU) return E_EQU_E;
                // E -> E ~= E
                else if (item2->symbol == NEQ) return E_NEQ_E;
            
            else {
                // E -> ( E )
                if (item1->symbol == LBR && item2->symbol == NONTERM && item3->symbol == RBR ) return LBR_E_RBR;

            }
        
        return ND_RULE;

        default: 

        return ND_RULE;

    }           

}

int test_semantic ( Stack_item *item1, Stack_item *item2, Stack_item *item3, pt_rule rule ) {

    switch (rule) {

        case E_RULE:

            if (item1->token.type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item1->token.type == T_NIL) exit( ERR_NIL );
            
        return item1->token.type;

        case LEN_E:

            if (item2->token.type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item2->token.type == T_NIL) exit( ERR_NIL );
            if (item2->token.type != T_STR && item2->token.type != T_IDE) exit( ERR_SEMANTIC_INCOP_EXP );
        
        return INT;

        case E_PLUS_E:
        case E_MINUS_E:
        case E_MUL_E:

            if (item1->token.type == T_NDA || item3->token.type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item1->token.type == T_NIL || item3->token.type == T_NIL) exit( ERR_NIL );
            if (item1->token.type == T_INT && item3->token.type == T_INT) return INT;
            else if (item1->token.type == T_NUM && item3->token.type == T_NUM) return NUM;
            else {
                    // TODO
                if        (item1->token.type == T_INT) {    // convert 1. operand to num
                    
                } else if (item3->token.type == T_INT) {    // convert 2. operand to num
                    
                }

            }

        return NUM;

        case E_DIV_E:

            if (item1->token.type == T_NDA || item3->token.type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item1->token.type == T_NIL || item3->token.type == T_NIL) exit( ERR_NIL );
            if (item1->token.type != T_NUM || item3->token.type != T_NUM) exit( ERR_SEMANTIC_INCOP_EXP );
            else {

                if        (item1->token.type == T_INT) {    // convert 1. operand to num
                    
                } else if (item3->token.type == T_INT) {    // convert 2. operand to num
                    
                }

            }
            // to convert or to not convert? atm - no

        return NUM;

        case E_IDIV_E:

            if (item1->token.type == T_NDA || item3->token.type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item1->token.type == T_NIL || item3->token.type == T_NIL) exit( ERR_NIL );
            if (item1->token.type != T_INT || item3->token.type != T_INT) exit( ERR_SEMANTIC_INCOP_EXP );
            else {

                if        (item1->token.type == T_NUM) {    // convert 1. operand to int
                    
                } else if (item3->token.type == T_NUM) {    // convert 2. operand to int
                    
                }

            }

        return INT;

        case LBR_E_RBR:

            if (item2->token.type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item2->token.type == T_NIL) exit( ERR_NIL );
        
        return item2->token.type;

        case E_CAT_E:

            if (item1->token.type == T_NDA || item3->token.type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item1->token.type == T_NIL || item3->token.type == T_NIL) exit( ERR_NIL );
            if (item1->token.type != T_STR && item1->token.type != T_IDE
             || item3->token.type != T_STR && item3->token.type != T_IDE) exit( ERR_SEMANTIC_INCOP_EXP );

        return STR;

        case E_LTH_E:
        case E_LET_E:
        case E_MTH_E:
        case E_MET_E:

            if (item1->token.type == T_NDA || item3->token.type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item1->token.type == T_NIL || item3->token.type == T_NIL) exit( ERR_NIL );
            if (item1->token.type != item3->token.type) {
                if (item1->token.type > item3->token.type) {
                    // convert 1. to NUM
                }
                else if (item3->token.type < item3->token.type) {
                    // convert 2. to NUM
                }
            }

        return BOO;
        
        case E_EQU_E:
        case E_NEQ_E:

            if (item1->token.type == T_NDA || item3->token.type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item1->token.type != T_NIL && item3->token.type != T_NIL) {
                if (item1->token.type != item3->token.type) {
                    if (item1->token.type > item3->token.type) {
                        // convert 1. to NUM
                    }
                    else if (item3->token.type < item3->token.type) {
                        // convert 2. to NUM
                    }
                }

            }

        return BOO;

    }


}

/**
 * @brief Saves expression result 
 * @param parserData current data
 * @param stack stack for symbols
 */
int save_result ( Parser_data *parserData, Stack *stack ) {

    if (!parserData->lhsId) return 0;

    int res = 0;

    Stack_item *result = s_top( stack );
    // check variable's and expression's types compatibility
    switch (result->symbol) {

        case (INT) :

            if (parserData->lhsId->type == NUM) {
                cg_convert_top_num2int(  );
            }            

            cg_save_result( parserData->lhsId->id );

        break;

        case (T_NUM):

            if (parserData->lhsId->type == INT) {
                cg_convert_top_int2num(  );
            }

            cg_save_result( parserData->lhsId->id );

        break;

        case (T_STR):

            if (parserData->lhsId->type != T_STR) exit( ERR_SEMANTIC_INCOP_TYPE );

            cg_save_result( parserData->lhsId->id );

        break;

        case (T_BOO):

            if (parserData->lhsId->type != T_BOO) exit( ERR_SEMANTIC_INCOP_TYPE );

            cg_save_result( parserData->lhsId->id );

        break;

        default:
        break;
        
    }

}