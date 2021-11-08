/**
 *  PROJECT: Implementation of imperative language translator (IFJ21)
 *  PART: RECURSIVE TOP-DOWN PARSER
 *
 *  AUTHOR(S): xtikho00, Maksim Tikhonov
 * 
 * 
 * 
*/
#ifndef PARSER_C
#define PARSER_C

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "errorslist.h"
#include "scanner.c" 
#include "symtable.c"
#include "code_generator.c"


int get_next_token_and_check_type ( Parser_data *parserData, Token_type type ) {

    int tmp;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    printf( "(%d -)\n", parserData->token.type );
    if (parserData->token.type != type) return ERR_SYNTAX;

    return 0;

}

int get_next_token_and_check_keyword ( Parser_data *parserData, Keyword keyword ) {

    int tmp;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    printf( "(%d %d)\n", parserData->token.type, parserData->token.attribute.keyword );
    if (parserData->token.type != TT_KEY
        || parserData->token.attribute.keyword != keyword) return ERR_SYNTAX;
    
    return 0;

}

int get_next_token_and_apply_rule ( Parser_data *parserData, char *rulename ) {

    int tmp;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    printf( "(%d %d)\n", parserData->token.type, parserData->token.attribute.keyword );
    if      (!strcmp( rulename, "rule_paramList" )) return rule_paramList( parserData );
    //else if (!strcmp( rulename, "rule_Expression" )) return rule_Expression( parserData );
    else if (!strcmp( rulename, "rule_statementList")) return rule_statementList( parserData );
    else if (!strcmp( rulename, "rule_Write" )) return rule_Write( parserData );

}

bool parser_data_init ( Parser_data *parserData ) {

    int res;

    // parser data parameters

    parserData->in_func = 0;
    parserData->if_decl = 0;
    parserData->in_decl = 0;
    parserData->in_loop = 0;

    parserData->lhsId = NULL;
    parserData->rhsId = NULL;
    parserData->currentId = NULL;
    parserData->auxId = NULL;
    
    parserData->labelId = 0;
    parserData->paramId = 0;
    parserData->labelDepth = 0;
    parserData->whereAmI = 0;
    
    // symTable[0] - global table; all others are local ones
    st_init( &parserData->symTable[0] );

    // built-in functions
    Item_data *tmp;

    tmp = st_add_id( &parserData->symTable[0], "reads" );
    if (res = st_add_param( tmp->outputTypes, KW_STRING )) return res;
    tmp->ifdec = 1;
    tmp->ifdef = 1;
    
    tmp = st_add_id( &parserData->symTable[0], "readi" );
    if (res = st_add_param( tmp->outputTypes, KW_INTEGER )) return res;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    tmp = st_add_id( &parserData->symTable[0], "readn" );
    if (res = st_add_param( tmp->outputTypes, KW_NUMBER )) return res;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    tmp = st_add_id( &parserData->symTable[0], "write" );
    if (res = st_add_param( tmp->inputTypes, KW_NUMBER )) return res;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    tmp = st_add_id( &parserData->symTable[0], "tointeger" );
    if (res = st_add_param( tmp->outputTypes, KW_NUMBER )) return res;
    if (res = st_add_param( tmp->inputTypes, KW_INTEGER )) return res;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    tmp = st_add_id( &parserData->symTable[0], "substr" );
    if (res = st_add_param( tmp->inputTypes, KW_STRING)) return res;
    if (res = st_add_param( tmp->inputTypes, KW_NUMBER )) return res;
    if (res = st_add_param( tmp->inputTypes, KW_NUMBER )) return res;
    if (res = st_add_param( tmp->outputTypes, KW_STRING )) return res;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    tmp = st_add_id( &parserData->symTable[0], "ord" );
    if (res = st_add_param( tmp->inputTypes, KW_STRING)) return res;
    if (res = st_add_param( tmp->inputTypes, KW_INTEGER )) return res;
    if (res = st_add_param( tmp->outputTypes, KW_INTEGER )) return res;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    tmp = st_add_id( &parserData->symTable[0], "chr" );
    if (res = st_add_param( tmp->inputTypes, KW_INTEGER )) return res;
    if (res = st_add_param( tmp->outputTypes, KW_STRING )) return res;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    return true;

}



int parse (  ) {
    
    int res;

    Dynamic_string str;
    Dynamic_string *dynamicString = &str;
    if (!ds_init( dynamicString )) return ERR_INTERNAL;
    _dynamic_string( dynamicString );
    Parser_data pData;
    Parser_data *parserData = &pData;
    if (!parser_data_init( parserData )) {
        ds_free( dynamicString );
        return ERR_INTERNAL;
    }

    // general rule: 
    // <program> -> <prologue>.<function list>
    // <prologue>
    if (res = rule_prologue( &pData )) return res;
    // <function list>
    if (res = rule_functionList( &pData )) return res;

    printf( "%d\n", res );

    return res; 

}

/*........................................ RECURSIVE TOP-DOWN PARSER ........................................ */

/**
 * <prologue> -> require "ifj21"
 */
int rule_prologue ( Parser_data *parserData ) {

    int tmp;
    if (tmp = get_next_token_and_check_keyword ( parserData, KW_REQUIRE )) return tmp;

    if (tmp = get_next_token( &parserData->token )) return tmp;
    printf( "(%d %s)\n", parserData->token.type, parserData->token.attribute.string->str );
    if (strcmp(parserData->token.attribute.string->str, "ifj21") != 0) return ERR_SYNTAX;
    // token for next stage
    if (tmp = get_next_token( &parserData->token )) return tmp;

    return 0;

}

/**
    // <function list> -> <function declaration>.<function list>
    // <function list> -> <function declaration>
    // <function list> -> <function definition>.<function list>
    // <function list> -> <function definition>
    // <function list> -> ε
 */ 
int rule_functionList ( Parser_data *parserData ) {

    int res;

    // <function declaration> -> GLOBAL ID : FUNCTION (<param list type 1>) : <param list type 3> . <function list>
    if (parserData->token.attribute.keyword == KW_GLOBAL) {                                     // GLOBAL

        if (res = get_next_token_and_check_type( parserData, TT_IDE )) return res;              // ID

        parserData->currentId = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
        if (parserData->currentId == NULL || parserData->currentId->ifdec == 0) {
            parserData->currentId = st_add_id( &parserData->symTable[0], parserData->token.attribute.string->str );
            if (!&parserData->currentId) return ERR_INTERNAL;
        } else return ERR_SEMANTIC_UNDEF_VAR;

        if (res = get_next_token_and_check_type( parserData, TT_COL )) return res;              // :
        if (res = get_next_token_and_check_keyword( parserData, KW_FUNCTION )) return res;      // FUNCTION
        if (res = get_next_token_and_check_type( parserData, TT_LBR )) return res;              // (
        
        parserData->whereAmI = 1;
        if (res = get_next_token_and_apply_rule( parserData, "rule_paramList" )) return res;    // <param list type 1>
        
        printf( "(%d -)\n", parserData->token.type );
        if (parserData->token.type != TT_RBR) return ERR_SYNTAX;                                // )
        if (res = get_next_token_and_check_type( parserData, TT_COL )) return res;              // :

        parserData->whereAmI = 3;
        if (res = get_next_token_and_apply_rule( parserData, "rule_paramList" )) return res;    // <param list type 3>
        
        return rule_functionList( parserData );                                                 // <function list>

    // <function definition> -> FUNCTION ID (<param list type 2>) : <param list type 3> <statement list> END . <function list>
    } else if (parserData->token.attribute.keyword == KW_FUNCTION) {                            // FUNCTION

        if (res = get_next_token_and_check_type( parserData, TT_IDE )) return res;              // ID

        parserData->currentId = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
        if (parserData->currentId == NULL || parserData->currentId->ifdef == 0) {
            parserData->currentId = st_add_id( &parserData->symTable[0], parserData->token.attribute.string->str );
            if (!&parserData->currentId) return ERR_INTERNAL;
        }
        else return ERR_SEMANTIC_UNDEF_VAR;

        parserData->currentId->ifdef = 1;
        parserData->currentId->ifdec = 1;
        
        // generate function header code
        cg_function_header( parserData->currentId->id );

        if (res = get_next_token_and_check_type( parserData, TT_LBR )) return res;              // (
        
        parserData->currentDepth++;
        st_init( &parserData->symTable[parserData->currentDepth] );
        parserData->whereAmI = 2;
        if (res = get_next_token_and_apply_rule( parserData, "rule_paramList" )) return res;    // <param list type 2>
         
        if (parserData->token.type != TT_RBR) return ERR_SYNTAX;                                // )

        if (res = get_next_token_and_check_type( parserData, TT_COL )) return res;              // :

        parserData->whereAmI = 3;
        if (res = get_next_token_and_apply_rule( parserData, "rule_paramList" )) return res;    // <param list type 3>
        
        if (res = rule_statementList( parserData )) return res;                                 // <statement list>
        st_dispose( parserData->symTable[parserData->currentDepth].rootItem );
        parserData->currentDepth--;

        if (res = get_next_token_and_check_keyword( parserData, KW_END )) return res;           // END

        if (res = get_next_token( &parserData->token )) return res;
        return rule_functionList( parserData );                                                 // <function list>

    }

    // <function list> -> ε
    else {
        
        return 0;

    }

}

/**
 * <param list> -> <param>.<param list>
 * <param list> -> <param>
 */ 
int rule_paramList ( Parser_data *parserData ) {

    int res;
    // if there is declared function or variable in the same context lvl with same identifier
    if (parserData->token.type == TT_IDE && st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str ) != NULL
    && st_search( parserData->symTable[parserData->currentDepth].rootItem, parserData->token.attribute.string->str ) != NULL)
        return ERR_SEMANTIC_UNDEF_VAR;

    if (parserData->token.type == TT_IDE || parserData->token.type == TT_KEY) {
        // <param> -> TYPE, (in function header: case 1: input types; case 2: output types)
        if (parserData->whereAmI == 1 || parserData->whereAmI == 3) { 

            if (parserData->token.type == TT_IDE) return ERR_SYNTAX;                            // TYPE

            if      (parserData->whereAmI == 1) {
                if (res = st_add_param( parserData->currentId->inputTypes, parserData->token.attribute.keyword )) return res;
            }
            else if (parserData->whereAmI == 3) {
                if (res = st_add_param( parserData->currentId->outputTypes, parserData->token.attribute.keyword )) return res;
                cg_function_output_type( parserData->token.attribute.keyword, parserData->currentId->outputTypes->length - 1 );
            }

            if (res = get_next_token_and_check_type( parserData, TT_COM )) return res;          // ,    

        }
        // <param> -> ID : TYPE, (in function defenition)
        else if (parserData->whereAmI == 2) {
            
            if (parserData->token.type == TT_KEY) return ERR_SYNTAX;                            // ID

            parserData->auxId = st_add_id( &parserData->symTable[parserData->currentDepth], parserData->token.attribute.string->str );

            if (res = get_next_token_and_check_type( parserData, TT_COL )) return res;          // :
            if (res = get_next_token( &parserData->token )) return res;                         // TYPE

            if (res = st_add_type( &parserData->token, parserData->auxId )) return res;
            cg_function_input_type( parserData->currentId, parserData->token.attribute.keyword, parserData->currentId->inputTypes->length );

            if (res = get_next_token_and_check_type( parserData, TT_COM )) return res;          // ,

        }

        if (parserData->token.type != TT_RBR) if (res = get_next_token( &parserData->token )) return res;
        return rule_paramList( parserData ); 

    }

    else {

        return 0;

    }

}

/**
 * <statement list> -> <statement>.<statement list>
 * <statement list> -> <statement>
 * <statement list> -> ε
 * <statement> -> ... (see cases)
 * <statement> -> ε
 */
int rule_statementList ( Parser_data *parserData ) {

    int res;
    // <ID list> = <value>, <value>, ..., <value>
    // <ID list> = ID (function with many output types)
    if (parserData->token.type == TT_IDE) {
        
        



        if (res = get_next_token( &parserData->token )) return res;
        return rule_statementList( parserData );                                                // <statement list>

    }
    // <statement> -> LOCAL ID : TYPE = <value> . <statement list>
    // <statement> -> LOCAL ID : TYPE . <statement list>
    else if (parserData->token.attribute.keyword == KW_LOCAL) {                                 // LOCAL

        if (res = get_next_token_and_check_type( parserData, TT_IDE )) return res;              // ID
        parserData->currentId = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
        parserData->currentId = st_search( parserData->symTable[parserData->currentDepth].rootItem, parserData->token.attribute.string->str );
        if (!parserData->currentId)
            parserData->currentId = st_add_id( &parserData->symTable[parserData->currentDepth], parserData->token.attribute.string->str );
        else return ERR_SEMANTIC_UNDEF_VAR;
        if (res = get_next_token_and_check_type( parserData, TT_COL )) return res;              // :
        if (res = get_next_token_and_check_type( parserData, TT_KEY )) return res;              // TYPE
        if (res = st_add_type( &parserData->token, parserData->currentId )) return res;
        if (res = get_next_token( &parserData->token )) return res;                             // 
        
        if (parserData->token.type == TT_EQU)                                                   // <value>
            if (res = get_next_token_and_apply_rule( parserData, "rule_Value" )) return res;
        return rule_statementList( parserData );                                                // <statement list>

    }
    // <statement> -> IF <expression> THEN <statement list> ELSE <statement list> END . <statement list>
    else if (parserData->token.attribute.keyword == KW_IF) {                                    // IF
        
        parserData->currentDepth++;
        parserData->in_loop = true;

        if (res = get_next_token_and_apply_rule( parserData, "rule_Expression" )) return res;   // <expression>
        if (res = get_next_token_and_check_keyword( parserData, KW_THEN )) return res;          // THEN
        if (res = get_next_token_and_apply_rule( parserData, "rule_statementList" )) return res;// <statement list>
        if (res = get_next_token_and_check_keyword( parserData, KW_ELSE )) return res;          // ELSE
        if (res = get_next_token_and_apply_rule( parserData, "rule_statementList" )) return res;// <statement list>
        if (res = get_next_token_and_check_keyword( parserData, KW_END )) return res;           // END

        parserData->currentDepth--;
        parserData->in_loop = false;

        if (res = get_next_token( &parserData->token )) return res;
        return rule_statementList( parserData );                                                // <statement list>

    }

    // <statement> -> WhILE <expression> DO <statement list> END . <statement list>
    else if (parserData->token.attribute.keyword == KW_WHILE) {                                 // WHILE
        
        parserData->currentDepth++;
        parserData->in_loop = true;
        
        if (res = get_next_token_and_apply_rule( parserData, "rule_Expression" )) return res;   // <expression>
        if (res = get_next_token_and_check_keyword( parserData, KW_DO )) return res;            // DO
        if (res = get_next_token_and_apply_rule( parserData, "rule_statementList" )) return res;// <statement list>
        if (res = get_next_token_and_check_keyword( parserData, KW_END )) return res;           // END

        parserData->currentDepth--;
        parserData->in_loop = false;

        if (res = get_next_token( &parserData->token )) return res;
        return rule_statementList( parserData );                                                // <statement list>

    }

    // <statement> -> RETURN <expression list>
    // <expression list> -> <expression> . <expression list>
    // <expression list> -> <expression>
    else if (parserData->token.attribute.keyword == KW_RETURN) {
        
        if (res = get_next_token_and_apply_rule( parserData, "rule_Expression" )) return res;   // <expression>

        return rule_statementList( parserData );                                                // <statement list>

    }

    // <statement> -> write (string_ID1, stringID2, ... , stringIDN)
    else if (parserData->token.attribute.keyword == KW_WRITE) {                                 // WRITE

        if (res = get_next_token_and_check_type( parserData, TT_LBR )) return res;              // (
        if (res = get_next_token_and_apply_rule( parserData, "rule_Write" )) return res;        // string_ID,
        if (res = get_next_token_and_check_type( parserData, TT_RBR )) return res;              // )

    }

    
    // <statement> -> ε
    else return 0;

}

int rule_Write ( Parser_data *parserData ) {

    int res;

    if (parserData->token.type == TT_STR || parserData->token.type == TT_IDE) {                 // string_ID or string                                 

        if (parserData->token.type == TT_IDE) {
            // if identifier is function
            parserData->auxId = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
            if (parserData->auxId != NULL) { // should have single type string as variable
                if (strcmp( parserData->auxId->outputTypes->str, "s" )) return ERR_SEMANTIC_INCOP_TYPE;
            // ... is variable
            } else {
                for (int i = parserData->currentDepth; i > 0; i--) {
                    parserData->auxId = st_search( parserData->symTable[i].rootItem, parserData->token.attribute.string->str );
                    if (parserData->auxId != NULL) break;
                }
                if (parserData->auxId->type != IT_STR) return ERR_SEMANTIC_INCOP_TYPE;
            }
            
        }
        
        else {
            

        }
        


        return rule_Write( parserData );

    } else return 0;

}
/**
 * <value> -> ID (either function or variable)
 * <value> -> <expression>
 */
int rule_Value ( Parser_data *parserData ) {

    int res;

    if (parserData->token.type == TT_IDE || parserData->token.type == TT_KEY) {

        if (parserData->token.type == TT_KEY) {
            
            switch (parserData->token.attribute.keyword) {

                case KW_READS:

                    parserData->rhsId = st_search( parserData->symTable[0].rootItem, "reads" );

                break;
            
                case KW_READI:
                
                    parserData->rhsId = st_search( parserData->symTable[0].rootItem, "readi" );

                break;

                case KW_READN:
                
                    parserData->rhsId = st_search( parserData->symTable[0].rootItem, "readn" );

                break;

                case KW_TOINTEGER:

                    parserData->rhsId = st_search( parserData->symTable[0].rootItem, "tointeger" );

                break;

                case KW_SUBSTR:
                
                    parserData->rhsId = st_search( parserData->symTable[0].rootItem, "substr" );

                break;

                case KW_ORD:
                
                    parserData->rhsId = st_search( parserData->symTable[0].rootItem, "ord" );

                break;

                case KW_CHR:
                
                    parserData->rhsId = st_search( parserData->symTable[0].rootItem, "chr" );

                break;

                default: 

                    return ERR_SYNTAX;

                break;

            }

        }
    
        else if (parserData->token.type == TT_IDE) {
            // <value> -> function_ID
            parserData->rhsId = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
            // <value> -> variable_ID
            if (parserData->rhsId == NULL)
                for (int i = parserData->currentDepth; i > 0; i--) {
                    parserData->rhsId = st_search( parserData->symTable[i].rootItem, parserData->token.attribute.string->str );
                    if (parserData->rhsId != NULL) break;
                }
        
        }
        // <value> -> function_ID (built-in function)
        

        

    }
    // <value> -> <expression>
    else {

    }

}


#endif