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
#include "scanner.h" 
#include "symtable.c"
#include "code_generator.c"


int get_next_token_and_check_type ( Parser_data *parserData, Data_type type ) {
    
    int tmp;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    printf( "(%d -) ", parserData->token.type );
    if (parserData->token.type != type) return ERR_SYNTAX;
    
    return 0;

}

int get_next_token_and_give_type ( Parser_data *parserData ) {
    
    int tmp;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    printf( "(%d -) ", parserData->token.type );
    return parserData->token.type;

}

int get_next_token_and_check_keyword ( Parser_data *parserData, Keyword keyword ) {

    int tmp;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    printf( "(%d %d) ", parserData->token.type, parserData->token.attribute.keyword );
    if (parserData->token.type != T_KEY
        || parserData->token.attribute.keyword != keyword) return ERR_SYNTAX;
    
    return 0;

}


int get_next_token_and_give_keyword ( Parser_data *parserData ) {

    int tmp;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    printf( "(%d %d) ", parserData->token.type, parserData->token.attribute.keyword );
    return parserData->token.attribute.keyword;

}

int get_next_token_and_apply_rule ( Parser_data *parserData, char *rulename ) {
    
    int tmp;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    printf( "(%d -) ", parserData->token.type );

    if      (!strcmp( rulename, "rule_paramList" )) {
        if (tmp = rule_paramList( parserData )) return tmp;
    }
    else if (!strcmp( rulename, "rule_Expression" )) {
        if (tmp = rule_Expression( parserData )) return tmp;
    }
    else if (!strcmp( rulename, "rule_expressionList" ))  {
        if (tmp = rule_expressionList( parserData )) return tmp;
    }
    else if (!strcmp( rulename, "rule_statementList")) {
        if (tmp = rule_statementList( parserData )) return tmp;
    }
    
    else return ERR_SYNTAX;

}

bool parser_data_init ( Parser_data *parserData ) {

    int res;

    // parser data parameters

    parserData->inFunc = 0;
    parserData->inDecl = 0;
    parserData->inLoop = 0;

    parserData->lhsId = NULL;
    parserData->rhsId = NULL;
    parserData->currentId = NULL;
    parserData->auxId = NULL;
    
    parserData->paramIndex = 0;
    parserData->currentDepth = 0;
    parserData->whereAmI = 0;
    parserData->idCounter = 0;

    
    // symTable[0] - global table; all others are local ones
    st_init( &parserData->symTable[0] );

    // built-in functions
    Item_data *tmp;

    tmp = st_add_id( &parserData->symTable[0], "reads" );
    if (res = st_add_param( tmp->outputTypes, KW_STRING )) return false;
    tmp->ifdec = 1;
    tmp->ifdef = 1;
    
    tmp = st_add_id( &parserData->symTable[0], "readi" );
    if (res = st_add_param( tmp->outputTypes, KW_INTEGER )) return false;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    tmp = st_add_id( &parserData->symTable[0], "readn" );
    if (res = st_add_param( tmp->outputTypes, KW_NUMBER )) return false;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    tmp = st_add_id( &parserData->symTable[0], "write" );
    if (res = st_add_param( tmp->inputTypes, KW_NUMBER )) return false;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    tmp = st_add_id( &parserData->symTable[0], "tointeger" );
    if (res = st_add_param( tmp->outputTypes, KW_NUMBER )) return false;
    if (res = st_add_param( tmp->inputTypes, KW_INTEGER )) return false;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    tmp = st_add_id( &parserData->symTable[0], "substr" );
    if (res = st_add_param( tmp->inputTypes, KW_STRING)) return false;
    if (res = st_add_param( tmp->inputTypes, KW_NUMBER )) return false;
    if (res = st_add_param( tmp->inputTypes, KW_NUMBER )) return false;
    if (res = st_add_param( tmp->outputTypes, KW_STRING )) return false;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    tmp = st_add_id( &parserData->symTable[0], "ord" );
    if (res = st_add_param( tmp->inputTypes, KW_STRING)) return false;
    if (res = st_add_param( tmp->inputTypes, KW_INTEGER )) return false;
    if (res = st_add_param( tmp->outputTypes, KW_INTEGER )) return false;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    tmp = st_add_id( &parserData->symTable[0], "chr" );
    if (res = st_add_param( tmp->inputTypes, KW_INTEGER )) return false;
    if (res = st_add_param( tmp->outputTypes, KW_STRING )) return false;
    tmp->ifdec = 1;
    tmp->ifdef = 1;

    return true;

}



int parse (  ) {
    
    int res;

    Dynamic_string str1;
    Dynamic_string *scannerString = &str1;
    if (!ds_init( scannerString )) return ERR_INTERNAL;
    _scanner_string( scannerString );

    Parser_data pData;
    Parser_data *parserData = &pData;
    if (!parser_data_init( parserData )) {
        ds_free( scannerString );
        return ERR_INTERNAL;
    }
    // general rule: 
    // <program> -> <prologue>.<function list>
    // <prologue>
    if (res = rule_prologue( parserData )) return res;
    Dynamic_string str2;
    Dynamic_string *codeString = &str2;;
    if (!ds_init( codeString )) return ERR_INTERNAL;
    _code_string( codeString );
    // <function list>
    if (res = rule_functionList( parserData )) return res;

    ds_free( scannerString );
    ds_free( codeString );

    printf( "END: %d ", res );

    return res; 

}

/*........................................ RECURSIVE TOP-DOWN PARSER ........................................ */

/**
 * <prologue> -> require "ifj21"
    @param parserData -- 
    @return error code
 */
int rule_prologue ( Parser_data *parserData ) {

    int tmp;
    if (tmp = get_next_token_and_check_keyword ( parserData, KW_REQUIRE )) return tmp;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    if (strcmp( parserData->token.attribute.string->str, "ifj21" ) != 0) return ERR_SYNTAX;

    if (tmp = get_next_token( &parserData->token )) return tmp;

    return 0;

}

/**
    // <function list> -> <function declaration>.<function list>
    // <function list> -> <function declaration>
    // <function list> -> <function definition>.<function list>
    // <function list> -> <function definition>
    // <function list> -> ε
    @param parserData -- 
    @return error code
 */ 
int rule_functionList ( Parser_data *parserData ) {

    int res;
    
    // <function declaration> -> GLOBAL ID : FUNCTION (<param list type 1>) : <param list type 3> . <function list>
    if (parserData->token.attribute.keyword == KW_GLOBAL) {                                     // GLOBAL

        if (res = get_next_token_and_check_type( parserData, T_IDE )) return res;               // ID
        
        parserData->currentId = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
        if (parserData->currentId == NULL || parserData->currentId->ifdec == 0) {
            parserData->currentId = st_add_id( &parserData->symTable[0], parserData->token.attribute.string->str );
            if (!&parserData->currentId) return ERR_INTERNAL;
        } else return ERR_SEMANTIC_UNDEF_VAR;

        if (res = get_next_token_and_check_type( parserData, T_COL )) return res;               // :
        if (res = get_next_token_and_check_keyword( parserData, KW_FUNCTION )) return res;      // FUNCTION
        if (res = get_next_token_and_check_type( parserData, T_LBR )) return res;               // (
        
        parserData->whereAmI = 1;
        if (res = get_next_token_and_apply_rule( parserData, "rule_paramList" )) return res;    // <param list type 1>
        
        printf( "(%d -) ", parserData->token.type );
        if (parserData->token.type != T_RBR) return ERR_SYNTAX;                                 // )
        if (res = get_next_token_and_check_type( parserData, T_COL )) return res;               // :

        parserData->whereAmI = 3;
        if (res = get_next_token_and_apply_rule( parserData, "rule_paramList" )) return res;    // <param list type 3>
        
        return rule_functionList( parserData );                                                 // <function list>

    // <function definition> -> FUNCTION ID (<param list type 2>) : <param list type 3> <statement list> END . <function list>
    } else if (parserData->token.attribute.keyword == KW_FUNCTION) {                            // FUNCTION
        
        if (res = get_next_token_and_check_type( parserData, T_IDE )) return res;               // ID
        
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
        
        if (res = get_next_token_and_check_type( parserData, T_LBR )) return res;               // (
        
        parserData->currentDepth++;
        st_init( &parserData->symTable[parserData->currentDepth] );

        parserData->whereAmI = 2;
        if (res = get_next_token_and_apply_rule( parserData, "rule_paramList" )) return res;    // <param list type 2>
        
        if (parserData->token.type != T_RBR) return ERR_SYNTAX;                                 // )

        if (res = get_next_token( &parserData->token )) return res;

        if (parserData->token.type == T_COL) {

            parserData->whereAmI = 3;
            if (res = get_next_token_and_apply_rule( parserData, "rule_paramList" )) return res;// <param list type 3>

        }
        if (res = rule_statementList( parserData )) return res;                                 // <statement list>
        
        st_dispose( &parserData->symTable[parserData->currentDepth].rootItem );
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
    @param parserData -- 
    @return error code
 */ 
int rule_paramList ( Parser_data *parserData ) {

    int res;
    // if there is declared function or variable in the same context lvl with same identifier
    if (parserData->token.type == T_IDE && st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str ) != NULL
    && st_search( parserData->symTable[parserData->currentDepth].rootItem, parserData->token.attribute.string->str ) != NULL)
        return ERR_SEMANTIC_UNDEF_VAR;

    if (parserData->token.type == T_IDE || parserData->token.type == T_KEY) {
        // <param> -> TYPE, (in function header: case 1: input types; case 2: output types)
        if (parserData->whereAmI == 1 || parserData->whereAmI == 3) { 

            if (parserData->token.type == T_IDE) return ERR_SYNTAX;                            // TYPE

            if      (parserData->whereAmI == 1) {
                if (res = st_add_param( parserData->currentId->inputTypes, parserData->token.attribute.keyword )) return res;
                
            }
            else if (parserData->whereAmI == 3) {
                if (res = st_add_param( parserData->currentId->outputTypes, parserData->token.attribute.keyword )) return res;
                cg_function_output_type( parserData->token.attribute.keyword, parserData->currentId->outputTypes->length - 1 );
            }

            if (res = get_next_token_and_check_type( parserData, T_COM )) return res;          // ,    

        }
        // <param> -> ID : TYPE, (in function defenition)
        else if (parserData->whereAmI == 2) {
            
            if (parserData->token.type == T_KEY) return ERR_SYNTAX;                            // ID

            parserData->auxId = st_add_id( &parserData->symTable[parserData->currentDepth], parserData->token.attribute.string->str );

            if (res = get_next_token_and_check_type( parserData, T_COL )) return res;          // :
            if (res = get_next_token( &parserData->token )) return res;                        // TYPE

            if (res = st_add_type( &parserData->token, parserData->auxId )) return res;
            cg_function_input_type( parserData->currentId->id, parserData->token.attribute.keyword, parserData->currentId->inputTypes->length );

            if (res = get_next_token_and_check_type( parserData, T_COM )) return res;          // ,

        }

        if (parserData->token.type != T_RBR) {
            if (res = get_next_token( &parserData->token )) return res;
        }
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
    @param parserData -- 
    @return error code
 */
int rule_statementList ( Parser_data *parserData ) {

    int res;
    // $1
    // <statement> -> <ID list> = <expression list>
    // <statement> -> <ID list> = ID (<param list>)
    if (parserData->token.type == T_IDE) {
        
        parserData->lhsId = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
        // if not function
        if (parserData->lhsId == NULL) { 

            /* while (1) { array of tokens variant
                
                parserData->lhsId = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
                if (parserData->lhsId == NULL) {
                    for (int i = parserData->currentDepth; i > 0; i--) {
                        parserData->lhsId = st_search( parserData->symTable[i].rootItem, parserData->token.attribute.string->str );
                        if (parserData->lhsId != NULL) break;
                    }
                    if (parserData->lhsId == NULL) return ERR_SEMANTIC_UNDEF_VAR; 
                } else return ERR_SEMANTIC_UNDEF_VAR;                                                   // you checked if it is declared already
                
                parserData->tokenTable[parserData->idCounter++] = parserData->token;
                
                if (res = get_next_token( &parserData->token )) return res;                             // , or =
                if (parserData->token.type == T_EQU) break;

            }

            for (int i = 0; i < parserData->idCounter; i++) {

                for (int j = parserData->currentDepth; j > 0; j--) {
                    parserData->lhsId = st_search( parserData->symTable[j].rootItem, parserData->tokenTable[i].attribute.string->str );
                    if (parserData->lhsId != NULL) break;
                }
                if (res = get_next_token_and_apply_rule( parserData, "rule_Value" )) return res;        // <value>

                if (res = get_next_token( &parserData->token )) return res;                             // ,
                if (parserData->token.type != T_COM) break;

            } 

            if (parserData->token.type == T_COM) return ERR_SEMANTIC_INCOP_TYPE;
            parserData->idCounter = 0; */

            
                
            for (int i = parserData->currentDepth; i > 0; i--) {
                parserData->lhsId = st_search( parserData->symTable[i].rootItem, parserData->token.attribute.string->str );
                if (parserData->lhsId != NULL) break;
                }
            if (parserData->lhsId == NULL) return ERR_SEMANTIC_UNDEF_VAR;                        // you checked if it is declared already

            if (res = get_next_token_and_check_type( parserData, T_EQU )) return res;

            if (res = get_next_token_and_apply_rule( parserData, "rule_Value" )) return res;

            parserData->lhsId = NULL;
            return rule_statementList( parserData );

        } 
        // <statement> -> function_ID (<param list>) 
        else {

            if (res = get_next_token_and_check_type( parserData, T_LBR )) return res;               // (

            if (parserData->lhsId->inputTypes->length > 0) {

                parserData->paramIndex = 0;
                while (parserData->lhsId->inputTypes->length >= parserData->paramIndex) {

                    (parserData->paramIndex)++;
                    int currentType = parserData->lhsId->inputTypes->str[parserData->paramIndex] - '0';
                    if (res = get_next_token( &parserData->token )) return res;
                    if (currentType != parserData->token.type) {
                        if (currentType == T_NUM && parserData->token.type != T_INT) {
                            return ERR_SEMANTIC_FUNC_PAR; 
                        }
                        else if (currentType == T_NUM && parserData->token.type == T_INT) {
                           // gc_int2numconvertion ()
                        }
                    }
                    


                }
            }

        }



        if (res = get_next_token( &parserData->token )) return res;
        return rule_statementList( parserData );                                                    // <statement list>

    }

    // $2
    // <statement> -> LOCAL ID : TYPE = <value> . <statement list>
    // <statement> -> LOCAL ID : TYPE . <statement list>
    else if (parserData->token.attribute.keyword == KW_LOCAL) {                                     // LOCAL

        if (res = get_next_token_and_check_type( parserData, T_IDE )) return res;                   // ID
        parserData->currentId = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
        parserData->currentId = st_search( parserData->symTable[parserData->currentDepth].rootItem, parserData->token.attribute.string->str );
        if (!parserData->currentId)
            parserData->currentId = st_add_id( &parserData->symTable[parserData->currentDepth], parserData->token.attribute.string->str );
        else return ERR_SEMANTIC_UNDEF_VAR;
        if (res = get_next_token_and_check_type( parserData, T_COL )) return res;                   // :
        if (res = get_next_token( &parserData->token )) return res;                                 // TYPE
        if (res = st_add_type( &parserData->token, parserData->currentId )) return res;
        printf( "(%d %d) ", parserData->token.type, parserData->token.attribute.keyword );
        if (res = get_next_token( &parserData->token )) return res;                                 // =

        if (parserData->token.type == T_EQU) {                                                      // <expression>
        
            if (res = get_next_token_and_apply_rule( parserData, "rule_Value" )) return res;
            
        }
        // will give you next token

        return rule_statementList( parserData );                                                    // <statement list>

    }

    // $3
    // <statement> -> IF <expression> THEN <statement list> ELSE <statement list> END . <statement list>
    else if (parserData->token.attribute.keyword == KW_IF) {                                        // IF
        
        parserData->currentDepth++;
        st_init( &parserData->symTable[parserData->currentDepth] );
        parserData->inLoop = true;

        if (res = get_next_token_and_apply_rule( parserData, "rule_Expression" )) return res;       // <expression>
        if (res = get_next_token_and_check_keyword( parserData, KW_THEN )) return res;              // THEN
        if (res = get_next_token_and_apply_rule( parserData, "rule_statementList" )) return res;    // <statement list>
        if (res = get_next_token_and_check_keyword( parserData, KW_ELSE )) return res;              // ELSE
        if (res = get_next_token_and_apply_rule( parserData, "rule_statementList" )) return res;    // <statement list>
        if (res = get_next_token_and_check_keyword( parserData, KW_END )) return res;               // END

        parserData->inLoop = false;
        st_dispose( &parserData->symTable[parserData->currentDepth].rootItem );
        parserData->currentDepth--;
        

        if (res = get_next_token( &parserData->token )) return res;
        return rule_statementList( parserData );                                                    // <statement list>

    }

    // $4
    // <statement> -> WhILE <expression> DO <statement list> END . <statement list>
    else if (parserData->token.attribute.keyword == KW_WHILE) {                                     // WHILE
        
        parserData->currentDepth++;
        st_init( &parserData->symTable[parserData->currentDepth] );
        parserData->inLoop = true;
        
        if (res = get_next_token_and_apply_rule( parserData, "rule_Expression" )) return res;       // <expression>
        if (res = get_next_token_and_check_keyword( parserData, KW_DO )) return res;                // DO
        if (res = get_next_token_and_apply_rule( parserData, "rule_statementList" )) return res;    // <statement list>
        if (res = get_next_token_and_check_keyword( parserData, KW_END )) return res;               // END

        parserData->inLoop = false;
        st_dispose( &parserData->symTable[parserData->currentDepth].rootItem );
        parserData->currentDepth--;

        if (res = get_next_token( &parserData->token )) return res;
        return rule_statementList( parserData );                                                    // <statement list>

    }

    // $5
    // <statement> -> RETURN <expression list>
    else if (parserData->token.attribute.keyword == KW_RETURN) {

        if (res = get_next_token_and_apply_rule( parserData, "rule_expressionList" )) return res;   // <expression>

        return rule_statementList( parserData );                                                    // <statement list>

    }

    // $6
    // <statement> -> WRITE (string_ID1, stringID2, ... , stringIDN) . <statement list>
    else if (parserData->token.attribute.keyword == KW_WRITE) {                                     // WRITE

        parserData->currentId = st_search( parserData->symTable[0].rootItem, "write" );
        //st_print( parserData->symTable[0].rootItem );
        if (res = get_next_token_and_check_type( parserData, T_LBR )) return res;                   // (

        while (parserData->token.type != T_RBR) {

            // generate frame
            cg_frame_to_pass_param(  );

            if (res = get_next_token( &parserData->token )) return res;

            if (parserData->token.type == T_IDE) {

                for (int i = parserData->currentDepth; i > 0; i--) {
                    parserData->auxId = st_search( parserData->symTable[i].rootItem, parserData->token.attribute.string->str);
                    if (parserData->auxId != NULL) break;
                }
                if (parserData->auxId == NULL) return ERR_SEMANTIC_UNDEF_VAR;

                cg_pass_param( &parserData->token, 1 );
                cg_call( "write" );

                if (res = get_next_token( &parserData->token )) return res;

                if (parserData->token.type == T_COM) {
                    ADD_LINE( "WRITE string@\\010" );
                } else return ERR_SYNTAX;
                
                ADD_LINE( "WRITE string@\\010" );

            } else if (parserData->token.type == T_STR) { // ' ' -> \032

                ADD_CODE( "WRITE string@");
                ADD_LINE( parserData->token.attribute.string->str );
                printf("%s", codeString->str );

            }

        }
    
        if (res = get_next_token( &parserData->token )) return res;
        return rule_statementList( parserData );

    }

    // $7
    // <statement> -> READI () . <statement list>
    else if (parserData->token.attribute.keyword == KW_READI) {

        

    }
    
    // $
    // <statement> -> ε
    else return 0;

}

int rule_Expression ( Parser_data *parserData ) {

    int res;
    
    if (res = get_next_token( &parserData->token )) return res;

    return res;

}

/**
 * 
 */
int rule_expressionList ( Parser_data *parserData ) {

    int res;

    if (res = get_next_token( &parserData->token )) return res;
    return res;

}
/**
 * <value> -> ID (either function or variable)
 * <value> -> <expression>
 */
int rule_Value ( Parser_data *parserData ) {

    int res;

    if (parserData->token.type == T_IDE) {

        // <value> -> function_ID
        parserData->rhsId = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
        // <value> -> variable_ID
        if (parserData->rhsId == NULL)
            for (int i = parserData->currentDepth; i > 0; i--) {
                parserData->rhsId = st_search( parserData->symTable[i].rootItem, parserData->token.attribute.string->str );
                if (parserData->rhsId != NULL) break;
            }

        if (parserData->rhsId != NULL) {

            if (parserData->currentId->type != parserData->rhsId->type) return ERR_SEMANTIC_INCOP_TYPE;

            
        } else return ERR_SEMANTIC_UNDEF_VAR;

    } else {

        if (res = rule_Expression( parserData )) return res;

    }
    
}


#endif