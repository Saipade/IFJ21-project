/**
 *  PROJECT: Implementation of imperative language compiler (IFJ21)
 *  PART: EXPRESSION PROCESSING
 *  AUTHOR(S): Maksim Tikhonov (xtikho00)
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
// S - shift; R - reduce; E - equal; B - blank (not defined rule or SUCCES ($ - $))
pt_operation precedenceTable[9][9] = {
//input|+- |*/ |rel| ( | ) | i | # |.. | $    // top of stack
	   { R , S , R , S , R , S , S , R , R }, // +-
	   { R , R , R , S , R , S , S , R , R }, // * / //
	   { S , S , B , S , R , S , S , S , R }, // rel
	   { S , S , S , S , E , S , S , S , B }, // (
	   { R , R , R , B , R , B , R , R , R }, // )
	   { R , R , R , B , R , B , R , R , R }, // i
       { R , R , R , S , R , S , S , R , R }, // # // (.. x #) = ????
       { S , S , R , S , R , S , S , S , R }, // ..
	   { S , S , S , S , B , S , S , S , B }, // $
};

int shift ( Parser_data *parserData, Stack *stack, Data_type type, pt_terminal symbol );
int reduce ( Parser_data *parserData );
pt_rule check_rule ( int count, Stack_item *item1, Stack_item *item2, Stack_item *item3 );
Data_type test_semantic ( Stack_item *item1, Stack_item *item2, Stack_item *item3, pt_rule rule );
pt_terminal convert_token_2_symbol ( Parser_data *parserData );
pt_terminal convert_id_2_symbol ( Parser_data *parserData );
int convert_token_2_type( Parser_data *parserData );
pt_index get_pt_index ( pt_terminal symbol );
int generate_operation ( pt_rule ruleName );
int save_result ( Parser_data *parserData, Stack *stack );

Stack st;
Stack *stack = &st;

/**
 * @brief Auxiliary function, converts token to symbol
 * @param parserData contains token information
 * @return symbol
 */
pt_terminal convert_token_2_symbol ( Parser_data *parserData ) {

    if (parserData->token.type == T_KEY || parserData->token.type >= DOL) {

        return DOL;

    }

    /* else if ( parserData->token.type == T_IDE) {
        
        return convert_id_2_symbol( parserData );

    } */

    else {

        return parserData->token.type;

    }

}

/**
 * @brief Auxiliary function, converts symbol table item to symbol
 * @param parserData contains item information
 * @return symbol
 */
pt_terminal convert_id_2_symbol ( Parser_data *parserData ) {

    int res;
    Item_data *data;
    Item_data *tmp = parserData->currentVar;
    SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
    if (res == 0) exit( ERR_SEMANTIC_UNDEF_VAR );
    data = parserData->currentVar;
    parserData->currentVar = tmp;

    switch (data->type) {

        case (T_INT):

            return INT;

        break;

        case (T_NUM):

            return NUM;

        break;

        case (T_STR):
        
            return STR;

        break;

        case (T_NIL):

            return NIL;

        break;

        default:

            return NDA;

        break;

    }

}

/**
 * @brief Auxiliary function, converts token to data type
 * @param parserData contains token information
 * @return data type
 */
int convert_token_2_type( Parser_data *parserData ) {

    int res = 0;

    Item_data *data;
    Item_data *tmp = parserData->currentVar;

    switch (parserData->token.type) {

        case (T_INT):
            return T_INT;
        case (T_NUM):
            return T_NUM;
        case (T_STR):
            return T_STR;
        case (T_BOO):
            return T_BOO;
        case (T_NIL):
            return T_NIL;
        case (T_IDE):
            SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
            if (res == 0) exit( ERR_SEMANTIC_UNDEF_VAR );
            data = parserData->currentVar;
            parserData->currentVar = tmp;
            return data->type;
        default:
            return T_NDA;

    }

}


/**
 * @brief Auxiliary function, converts given symbol to precedence table index
 * @param symbol symbol
 * @return precedence table index
 */
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

/**
 * @brief Applies <expression> rule
 * @param parserData contains all required information
 * @return error code
 */
int rule_expression ( Parser_data *parserData ) {
    
    int res = 0;
    
    s_init( stack );
    s_push( stack, T_DOL, DOL );
    
    pt_terminal firstTerminalSymbol;
    pt_terminal nextSymbol;
    Data_type nextType;
    pt_operation currentOperation;
    bool success = false;
    //printf("\n");
    
    while (!success) {

        if (stack->top) firstTerminalSymbol = s_top_terminal_symbol( stack );
        nextSymbol = convert_token_2_symbol( parserData );
        nextType = convert_token_2_type( parserData );
        currentOperation = precedenceTable[get_pt_index(firstTerminalSymbol)][get_pt_index(nextSymbol)];
        //printf("%d = %d x %d |", currentOperation, get_pt_index(firstTerminalSymbol), get_pt_index(nextSymbol));
        switch (currentOperation) {
            // Shift
            case S:
                
                if (res = shift( parserData, stack, nextType, nextSymbol )) return res;       
                if (res = get_next_token( &parserData->token )) return res;
                
            break;
            // Reduce
            case R:
                
                if (res = reduce( parserData )) return res;
                
            break;
            // Equal
            case E:
                // just push
                s_push( stack, nextType, nextSymbol );
                if (res = get_next_token( &parserData->token )) return res;

            break;
            // Blank space
            case B:
                while (s_top_terminal_symbol( stack ) != DOL) {
                    if (res = reduce( parserData )) exit( res );
                }
                firstTerminalSymbol = s_top_terminal_symbol( stack );
                if ((nextSymbol == T_IDE || nextSymbol == DOL || nextSymbol == RBR) && firstTerminalSymbol == DOL) {
                    success = true;
                    break;
                }
                else exit( ERR_SYNTAX );

            break;

        }

    }
    
    if (res = save_result( parserData, stack ));

    s_dispose( stack );

    return 0;

}
/**
 * @brief Precedence table operation, shift
 * @param parserData contains token information
 * @param stack stack for data to be pushed in
 * @param type type to be pushed
 * @param symbol symbol to be pushed
 * @return error code
 */
int shift ( Parser_data *parserData, Stack *stack, Data_type type, pt_terminal symbol ) {

    int res = 0;
    
    s_push_before_terminal( stack, T_NDA, STOP );
    s_push( stack, type, symbol );
    if (IS_I( symbol )) {

        int index = 0;
        if (parserData->token.type == T_IDE) {
            SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
            if (res == 0) exit( ERR_SEMANTIC_UNDEF_VAR );
            res = 0;
        }
        
        
        index = parserData->currentVar->depth;
        cg_push( &parserData->token, index );

    }
    
    return 0;
    
}
/**
 * @brief Precedence table operation, reduce
 * @param parserData contains 
 * 
 */
int reduce ( Parser_data *parserData ) {

    int res = 0;
    
    int count = 0;
    bool found = 0;

    pt_rule resRule;

    Stack_item *item1 = NULL;
    Stack_item *item2 = NULL;
    Stack_item *item3 = NULL;

    // count symbols before stop sign

    Stack_item *tmp = s_top( stack );
    
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
        resRule = check_rule( count, item1, item2, item3 );
    }

    else if (count == 2) {
        item1 = stack->top;
        item2 = stack->top->nextItem;
        item3 = NULL;
        resRule = check_rule( count, item1, item2, item3 );
    }

    else if (count == 3) {
        item1 = stack->top;
        item2 = stack->top->nextItem;
        item3 = stack->top->nextItem->nextItem;
        resRule = check_rule( count, item1, item2, item3 );
    }

    if (resRule == ND_RULE) exit( ERR_SYNTAX );
    
    Data_type resType = test_semantic( item1, item2, item3, resRule );
    
    if (res = generate_operation( resRule ));
    // pop out all "consumed" symbols and stop sign
    for (int i = 0; i <= count; i++) s_pop( stack );
    // and push created non-terminal
    s_push( stack, resType, NONTERM );

    return res;

}

/**
 * @brief Function checks if there is relevant rule for given items
 * @param count number of stack items before STOP sign 
 * @param item1 top item
 * @param item2 next after top 
 * @param item3 next after top after top
 * @return relevant rule
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
            if (item1->symbol == NONTERM && item3->symbol == NONTERM) {
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
            
            }

            else {
                // E -> ( E )
                if (item1->symbol == RBR && item2->symbol == NONTERM && item3->symbol == LBR ) return LBR_E_RBR;

            }
        
        return ND_RULE;

        default: 

        return ND_RULE;

    }           

}
/**
 * @brief Tests if semantics is correct (type compatibility)
 * @param item1 top item
 * @param item2 next after top 
 * @param item3 next after top after top
 * @param rule acquired rule
 * @return data type for entire expression
 */
Data_type test_semantic ( Stack_item *item1, Stack_item *item2, Stack_item *item3, pt_rule rule ) {

    switch (rule) {

        case E_RULE:
            
            if (item1->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item1->type == T_NIL) exit( ERR_NIL );
            
        return item1->type;

        case LEN_E:

            if (item2->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item2->type == T_NIL) exit( ERR_NIL );
            if (item2->type != T_STR && item2->type != T_IDE) exit( ERR_SEMANTIC_INCOP_EXP );
        
        return INT;

        case E_PLUS_E:
        case E_MINUS_E:
        case E_MUL_E:

            if      (item1->type == T_NDA || item3->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            else if (item1->type == T_NIL || item3->type == T_NIL) exit( ERR_NIL );
            else if (item1->type == T_INT && item3->type == T_INT) return INT;
            else if (item1->type == T_NUM && item3->type == T_NUM) return NUM;
            else {
                
                if        (item1->type == T_INT) {    // convert 1. operand to num
                    cg_convert_1st_int2num(  );
                } else if (item3->type == T_INT) {    // convert 2. operand to num
                    cg_convert_2nd_int2num(  );
                }

            }

        return NUM;

        case E_DIV_E:

            if      (item1->type == T_NDA || item3->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            else if (item1->type == T_NIL || item3->type == T_NIL) exit( ERR_NIL );
            else if (item1->type == T_NUM && item3->type == T_NUM) return NUM;
            else {

                if (item1->type == INT && item3->type == INT) {     // convert both operands to num
                    cg_convert_both_int2num(  );
                }
                else if (item1->type == T_INT) {                    // convert 1. operand to num
                    cg_convert_1st_int2num(  );
                } 
                else if (item3->type == T_INT) {                    // convert 2. operand to num
                    cg_convert_2nd_int2num(  );
                }

            }

        return NUM;

        case E_IDIV_E:

            if (item1->type == T_NDA || item3->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item1->type == T_NIL || item3->type == T_NIL) exit( ERR_NIL );
            if (item1->type == T_INT && item3->type == T_INT) return INT;
            else {

                if (item1->type == T_NUM && item3->type == T_NUM) { // converts both operands to int
                    cg_convert_both_int2num(  );
                }
                else if (item1->type == T_NUM) {                    // converts 1. operand to int
                    cg_convert_1st_num2int(  );
                } 
                else if (item3->type == T_NUM) {                    // converts 2. operand to int
                    cg_convert_2nd_num2int(  );
                }

            }

        return INT;

        case LBR_E_RBR:
            
            if (item2->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item2->type == T_NIL) exit( ERR_NIL );
        
        return item2->type;

        case E_CAT_E:

            if (item1->type == T_NDA || item3->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item1->type == T_NIL || item3->type == T_NIL) exit( ERR_NIL );
            if (item1->type != T_STR && item3->type != T_STR) exit( ERR_SEMANTIC_INCOP_EXP );

        return STR;

        case E_LTH_E:
        case E_LET_E:
        case E_MTH_E:
        case E_MET_E:

            if (item1->type == T_NDA || item3->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            else if (item1->type == T_NIL || item3->type == T_NIL) exit( ERR_NIL );
            else if (item1->type == T_STR && item3->type == T_STR) cg_lens_both(  );
            else if (item1->type != item3->type) {
                if (item1->type == T_NUM && item3->type == T_INT) {
                    cg_convert_1st_int2num(  );
                }
                else if (item1->type == T_INT && item3->type == T_NUM) {
                    cg_convert_2nd_int2num(  );
                } else exit( ERR_SEMANTIC_INCOP_EXP );
            }
            else return BOO;
        
        case E_EQU_E:
        case E_NEQ_E:

            if (item1->type == T_NDA || item3->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            else if (item1->type == T_STR && item3->type == T_STR) cg_lens_both(  );
            else if (item1->type != T_NIL && item3->type != T_NIL) {
                if (item1->type != item3->type) {
                    if (item1->type == T_NUM && item3->type == T_INT) {
                        cg_convert_1st_int2num(  );
                    }
                    else if (item1->type == T_INT && item3->type == T_NUM) {
                        cg_convert_2nd_int2num(  );
                    } else exit( ERR_SEMANTIC_INCOP_EXP );
                }

            } 
            else return BOO;

    }

}
/**
 * @brief Generates operation according to rule
 * @param ruleName given rule
 * @return error code
 */
int generate_operation ( pt_rule ruleName ) {

    int res = 0;

    switch (ruleName) {

        case (LEN_E):
            cg_lens(  );
        break;
        case (E_PLUS_E):
            cg_adds(  );
        break;
        case (E_MINUS_E):
            cg_subs(  );
        break;
        case (E_MUL_E):
            cg_muls(  );
        break;
        case (E_DIV_E):
            cg_divs(  );
        break;
        case (E_IDIV_E):
            cg_idivs(  );
        break;
        case (E_CAT_E):
            cg_cats(  );
        break;
        case (E_LTH_E):
            cg_lths(  );
        break;
        case (E_LET_E):
            cg_lets(  );
        break;
        case (E_MTH_E):
            cg_mths(  );
        break;
        case (E_MET_E):
            cg_mets(  );
        break;
        case (E_EQU_E):
            cg_equs(  );
        break;
        case (E_NEQ_E):
            cg_neqs(  );
        break;

    }

    return 0;

}

/**
 * @brief Saves expression result to left-hand side identifier
 * @param parserData contains required information
 * @param stack stack for symbols
 * @return Error code
 */
int save_result ( Parser_data *parserData, Stack *stack ) {

    cg_save_result(  );
    // if there is variable on the left side, send expression result to it
    if (!parserData->lhsId) return 0;

    int res = 0;
    pt_terminal resultType = s_top_type( stack );
    
    // check variable's and expression's types compatibility
    switch (resultType) {

        case (INT) :

            if (parserData->lhsId->type == NUM) {
                cg_convert_1st_int2num(  );
            }            
            
            cg_save_to( parserData );
            
        break;

        case (T_NUM):

            if (parserData->lhsId->type == INT) {
                cg_convert_1st_num2int(  );
            }

            cg_save_to( parserData );

        break;

        case (T_STR):

            if (parserData->lhsId->type != T_STR) exit( ERR_SEMANTIC_INCOP_TYPE );

            cg_save_to( parserData );

        break;

        case (T_BOO):

            if (parserData->lhsId->type != T_BOO) exit( ERR_SEMANTIC_INCOP_TYPE );

            cg_save_to( parserData );

        break;

        default:
        break;
        
    }

}
