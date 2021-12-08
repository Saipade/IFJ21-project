/**
 * PROJECT: Implementation of imperative IFJ21 programming language compiler
 * PART: Expression processing
 * 
 * AUTHOR(S): Maksim Tikhonov (xtikho00)
 *            Sadovskyi Dmytro (xsadov06)
 */

#include "errorslist.h"
#include "scanner.h"
#include "expression.h"
#include "code_generator.h"
#include "stack.h"

#include <stdbool.h>
#include <stdlib.h>

// symbol on top has higher priority than next symbol -> shift
// symbol on top has lower or same priority compared to the next symbol -> reduce
// S - shift; R - reduce; E - equal; B - blank (not defined rule or SUCCESS ($ - $))
pt_operation precedenceTable[9][9] = {
//input|+- |*/ |rel| ( | ) | i | # |.. | $    // top of stack
	   { R , S , R , S , R , S , S , R , R }, // +, -
	   { R , R , R , S , R , S , S , R , R }, // *, /, //
	   { S , S , B , S , R , S , S , S , R }, // rel
	   { S , S , S , S , E , S , S , S , B }, // (
	   { R , R , R , B , R , B , R , R , R }, // )
	   { R , R , R , B , R , B , R , R , R }, // i
       { R , R , R , S , R , S , S , R , R }, // # 
       { S , S , R , S , R , S , S , S , R }, // ..
	   { S , S , S , S , B , S , S , S , B }, // $
};

Stack st;
Stack *stack = &st;

/**
 * @brief Auxiliary function, checks if value of item on the top of the stack is zero
 * @param stack symbol stack
 * @return True if zero
 */
bool check_if_item_is_zero( Stack *stack );

/**
 * @brief Precedence table operation, shift
 * @param parserData contains token information
 * @param stack symbol stack
 * @param type type to be pushed
 * @param symbol symbol to be pushed
 * @param isZero if item value is zero
 * @return Error code
 */
int shift ( Parser_data *parserData, Stack *stack, Data_type type, pt_symbol symbol, bool isZero );

/**
 * @brief Precedence table operation, identifies relevant reduction rule for up to 3 top symbol stack items
 * @param parserData contains required information
 * @return Error code
 */
int reduce ( Parser_data *parserData );

/**
 * @brief Auxiliary function, identifies if there is relevant rule for up to 3 top symbols
 * @param count number of stack items before STOP sign 
 * @param item1 top item
 * @param item2 next after top 
 * @param item3 next after top after top
 * @return Relevant rule
 */
pt_rule test_rule ( int count, Stack_item *item1, Stack_item *item2, Stack_item *item3 );

/**
 * @brief Tests if expression semantics are correct (type compatibility) and returns expression result data type
 * @param item1 top item
 * @param item2 next after top 
 * @param item3 next after top after top
 * @param rule acquired rule
 * @return Data type for entire expression
 */
Data_type test_semantics ( Stack_item *item1, Stack_item *item2, Stack_item *item3, pt_rule rule );

/**
 * @brief Saves expression result to left-hand side identifier (or to expResult global variable if there is no lhs id)
 * @param parserData contains required information
 * @param stack stack for symbols
 * @return Error code
 */
int save_result ( Parser_data *parserData, Stack *stack );

pt_symbol convert_token2symbol ( Parser_data *parserData ) {
    
    if (parserData->token.type == T_KEY || parserData->token.type >= T_DOL) {

        return DOL;

    }

    else {
        
        return parserData->token.type;

    }

}

Data_type convert_token2type ( Parser_data *parserData ) {

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
            SEARCH_GLOBAL( parserData->token.attribute.string->str );
            if (res == 1) return T_KEY; // return something that will be considered as $
            SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
            if (res == 0) exit( ERR_SEMANTIC_UNDEF_VAR );
            data = parserData->currentVar;
            parserData->currentVar = tmp;
            return data->type;
        default:
            return T_NDA;

    }

}

bool check_if_zero ( Parser_data *parserData ) {
    if (parserData->token.type == T_INT && parserData->token.attribute.integer == 0) return true;
    else if (parserData->token.type == T_NUM && parserData->token.attribute.floating == 0.0) return true;
    else return false;

}

bool check_if_item_is_zero( Stack *stack ) {

    if (stack->top->isZero == true) return true;
    else return false;

}

pt_index get_pt_index ( pt_symbol symbol ) {

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

int rule_expression ( Parser_data *parserData ) {
    
    int res = 0;
    
    s_init( stack );
    s_push( stack, T_DOL, DOL, false );
    
    pt_symbol firstTerminalSymbol;
    pt_symbol nextSymbol;
    Data_type nextType;
    bool isZero = false;
    pt_operation currentOperation;
    bool success = false;
    
    while (!success) {

        if (stack->top) firstTerminalSymbol = s_top_terminal_symbol( stack );
        nextSymbol = convert_token2symbol( parserData );
        nextType = convert_token2type( parserData );
        isZero = check_if_zero( parserData );
        currentOperation = precedenceTable[get_pt_index(firstTerminalSymbol)][get_pt_index(nextSymbol)];
        switch (currentOperation) {
            // Shift
            case S:
                
                if ((res = shift( parserData, stack, nextType, nextSymbol, isZero ))) return res;       
                if ((res = get_next_token( &parserData->token ))) return res;
                
            break;
            // Reduce
            case R:
            
                if ((res = reduce( parserData ))) return res;
                
            break;
            // Equal
            case E:
                // just push
                s_push( stack, nextType, nextSymbol, false );
                if ((res = get_next_token( &parserData->token ))) return res;

            break;
            // Blank space
            case B:

                while (s_top_terminal_symbol( stack ) != DOL) {
                    if ((res = reduce( parserData ))) return res;
                }
                firstTerminalSymbol = s_top_terminal_symbol( stack );
                if (( nextSymbol == IDE || nextSymbol == DOL || nextSymbol == RBR ) && firstTerminalSymbol == DOL) {
                    success = true;
                    break;
                }
                else return ERR_SYNTAX;

            break;

        }

    }

    if ((res = save_result( parserData, stack ))) return ERR_SEMANTIC_INCOP_TYPE;
    
    s_dispose( stack );

    return 0;

}

int shift ( Parser_data *parserData, Stack *stack, Data_type type, pt_symbol symbol, bool isZero ) {

    int res = 0;
    
    s_push_before_terminal( stack, T_NDA, STOP );
    s_push( stack, type, symbol, isZero );
    if (IS_I( symbol )) {
        // find the depth of current variable
        int index = 0;
        if (parserData->token.type == T_IDE) {
            SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
            if (res == 0) exit( ERR_SEMANTIC_UNDEF_VAR );
            res = 0;
            index = parserData->currentVar->depth;
        }
        
        cg_push( &parserData->token, index );

    }
    
    return 0;
    
}

int reduce ( Parser_data *parserData ) {

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

    if (found == 0) return ERR_SYNTAX;

    if      (count == 1) {
        item1 = stack->top;
        item2 = NULL;
        item3 = NULL;
        resRule = test_rule( count, item1, item2, item3 );
    }

    else if (count == 2) {;
        item1 = stack->top;
        item2 = stack->top->nextItem;
        item3 = NULL;
        resRule = test_rule( count, item1, item2, item3 );
    }

    else if (count == 3) {
        item1 = stack->top;
        item2 = stack->top->nextItem;
        item3 = stack->top->nextItem->nextItem;
        resRule = test_rule( count, item1, item2, item3 );
    }

    if (resRule == ND_RULE) exit( ERR_SYNTAX );
    bool isZero = false;
    if (resRule == E_RULE) {
        isZero = check_if_item_is_zero( stack );
    }
    Data_type resType = test_semantics( item1, item2, item3, resRule );
    parserData->expType = resType;
    if (!generate_operation( resRule )) return ERR_INTERNAL;
    // pop out all "consumed" symbols and stop sign
    for (int i = 0; i <= count; i++) s_pop( stack );
    // and push created non-terminal
    s_push( stack, resType, NONTERM, isZero );

    return 0;

}

pt_rule test_rule ( int count, Stack_item *item1, Stack_item *item2, Stack_item *item3 ) {

    switch (count) {
        
        case 1:
            // E -> i
            if (IS_I( item1->symbol )) {
                return E_RULE;
            }

        return ND_RULE;

        case 2:
            // E -> #E
            if (item1->symbol == NONTERM && item2->symbol == LEN) {
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

Data_type test_semantics ( Stack_item *item1, Stack_item *item2, Stack_item *item3, pt_rule rule ) {

    switch (rule) {

        case E_RULE:
            
            if (item1->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            
        return item1->type;

        case LEN_E:
        
            if (item1->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            if (item1->type == T_NIL) exit( ERR_NIL );
            if (item1->type != T_STR ) exit( ERR_SEMANTIC_INCOP_EXP );

        return INT;

        case E_PLUS_E:
        case E_MINUS_E:
        case E_MUL_E:

            if      (item1->type == T_NDA || item3->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            else if (item1->type == T_NIL || item3->type == T_NIL) exit( ERR_NIL );
            else if (item1->type == T_INT && item3->type == T_INT) return INT;
            else if (item1->type == T_NUM && item3->type == T_NUM) return NUM;
            else {
                
                if (item1->type == T_INT && item3->type == T_NUM) {         // convert 1. operand to num
                    if (!cg_convert_1st_int2num(  )) exit( ERR_INTERNAL );
                } else if (item3->type == T_NUM && item3->type == T_INT) {  // convert 2. operand to num
                    if (!cg_convert_2nd_int2num(  )) exit( ERR_INTERNAL );
                }

                else exit( ERR_SEMANTIC_INCOP_EXP );

            }

        return NUM;

        case E_DIV_E:

            if      (item1->type == T_NDA || item3->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            else if (item1->type == T_NIL || item3->type == T_NIL) exit( ERR_NIL );
            else if (item1->isZero) exit( ERR_DIV_NUL );
            else if (item1->type == T_NUM && item3->type == T_NUM) return NUM;
            else {
            
                if (item1->type == T_INT && item3->type == T_INT) {         // convert both operands to num
                    if (!cg_convert_both_int2num(  )) exit( ERR_INTERNAL );
                }
                else if (item1->type == T_INT && item3->type == T_NUM) {    // convert 1. operand to num
                    if (!cg_convert_1st_int2num(  )) exit( ERR_INTERNAL );
                } 
                else if (item1->type == T_NUM && item3->type == T_INT) {    // convert 2. operand to num
                    if (!cg_convert_2nd_int2num(  )) exit( ERR_INTERNAL );
                }

                else exit( ERR_SEMANTIC_INCOP_EXP );

            }

        return NUM;

        case E_IDIV_E:

            if (item1->type == T_NDA || item3->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            else if (item1->type == T_NIL || item3->type == T_NIL) exit( ERR_NIL );
            else if (item1->isZero == true) exit ( ERR_DIV_NUL );
            if (item1->type == T_INT && item3->type == T_INT) return INT;
            else {

                if (item1->type == T_NUM && item3->type == T_NUM) { // converts both operands to int
                    if (!cg_convert_both_int2num(  )) exit( ERR_INTERNAL );
                }
                else if (item1->type == T_NUM) {                    // converts 1. operand to int
                    if (!cg_convert_1st_num2int(  )) exit( ERR_INTERNAL );
                } 
                else if (item3->type == T_NUM) {                    // converts 2. operand to int
                    if (!cg_convert_2nd_num2int(  )) exit( ERR_INTERNAL );
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
            if (item1->type != T_STR || item3->type != T_STR) exit( ERR_SEMANTIC_INCOP_EXP );

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
                    if (!cg_convert_2nd_int2num(  )) exit( ERR_INTERNAL );
                }
                else if (item1->type == T_INT && item3->type == T_NUM) {
                    if (!cg_convert_1st_int2num(  )) exit( ERR_INTERNAL );
                } else exit( ERR_SEMANTIC_INCOP_EXP );
            }
            
            return BOO;
        
        case E_EQU_E:
        case E_NEQ_E:

            if (item1->type == T_NDA || item3->type == T_NDA) exit( ERR_SEMANTIC_UNDEF_VAR );
            else if (item1->type == T_STR && item3->type == T_STR) cg_lens_both(  );
            else if (item1->type != T_NIL && item3->type != T_NIL) {
                if (item1->type != item3->type) {
                    if (item1->type == T_NUM && item3->type == T_INT) {
                        if (!cg_convert_2nd_int2num(  )) exit( ERR_INTERNAL );
                    }
                    else if (item1->type == T_INT && item3->type == T_NUM) {
                        if (!cg_convert_1st_int2num(  )) exit( ERR_INTERNAL );
                    } else exit( ERR_SEMANTIC_INCOP_EXP );
                }

            } 
        return BOO;

        default:
        exit( ERR_SEMANTIC_OTHER ); // undefined rule

    }

}

int save_result ( Parser_data *parserData, Stack *stack ) {

    // if there is ho variable to assign to -> return
    if (!parserData->lhsId) {
        cg_save_result(  );
        return 0;
    } 

    Data_type resultType = s_top_type( stack );
    
    // check variable's and expression's types compatibility
    switch (resultType) {

        case (T_INT):
            
            if (parserData->lhsId->type == T_INT || parserData->lhsId->type == T_NUM) {
                if (parserData->lhsId->type == T_NUM) {
                    if (!cg_convert_1st_int2num(  )) return ERR_INTERNAL;
                }            
                
                if (!cg_save_to( parserData )) return ERR_INTERNAL;

            } else exit( ERR_SEMANTIC_INCOP_TYPE );
            
        break;

        case (T_NUM):
        
            if (parserData->lhsId->type == T_NUM) {

                if (!cg_save_to( parserData )) return ERR_INTERNAL;

            } else return ERR_SEMANTIC_INCOP_TYPE;

        break;

        case (T_STR):

            if (parserData->lhsId->type != T_STR) return ERR_SEMANTIC_INCOP_TYPE;

            if (!cg_save_to( parserData )) return ERR_INTERNAL;

        break;

        case (T_BOO):

            if (parserData->lhsId->type != T_BOO) return ERR_SEMANTIC_INCOP_TYPE;

            if (!cg_save_to( parserData )) return ERR_INTERNAL;

        break;
        
        case (T_NIL):

            if (!cg_save_to( parserData )) return ERR_INTERNAL;

        break;

        default:
        break;
        
    }

    return 0;

}
