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

#include "errorslist.h"
#include "string_processor.h"
#include "parser.h"
#include "symtable.h"
#include "code_generator.h"
#include "expression.h"


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

    int res = 0;

    // parser data parameters

    parserData->inFunc = 0;
    parserData->inDecl = 0;
    parserData->inLoop = 0;

    parserData->lhsId = NULL;
    parserData->rhsId = NULL;
    parserData->currentFunc = NULL;
    parserData->currentVar = NULL;
    
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
    
    int res = 0;

    Dynamic_string str1;
    Dynamic_string *scannerString = &str1;
    if (!ds_init( scannerString )) return ERR_INTERNAL;
    _token_string( scannerString );

    Parser_data pData;
    Parser_data *parserData = &pData;
    if (!parser_data_init( parserData )) {
        exit( ERR_INTERNAL );
    }

    Dynamic_string str2;
    Dynamic_string *codeString = &str2;
    if (!ds_init( codeString )) return ERR_INTERNAL;
    _code_string( codeString );
    // generate start of program
    if (!cg_start(  )) return ERR_INTERNAL;
    
    // general rule: 
    // <program> -> <prologue>.<function list>
    // <prologue>
    if (res = rule_prologue( parserData )) return res;
    // <function list>
    if (res = rule_functionList( parserData )) return res;

    ds_free( scannerString );
    ds_free( codeString );
    // generate end of main function
    if (!cg_end(  )) return ERR_INTERNAL;

    printf( "END: %d \n", res );

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
    if (tmp = get_next_token_and_check_keyword ( parserData, KW_REQUIRE )) exit( tmp );
    if (tmp = get_next_token( &parserData->token )) return tmp;
    if (strcmp( parserData->token.attribute.string->str, "ifj21" ) != 0) exit( ERR_SYNTAX );
    printf( "(%d %s) ", parserData->token.type, parserData->token.attribute.string->str );
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

    int res = 0;
    
    // <function declaration> -> GLOBAL ID : FUNCTION (<param list type 1>) : <param list type 3>
    if (parserData->token.attribute.keyword == KW_GLOBAL) {                                     // GLOBAL

        if (res = get_next_token_and_check_type( parserData, T_IDE )) return res;               // ID
        
        parserData->currentFunc = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
        if (parserData->currentFunc == NULL || parserData->currentFunc->ifdec == 0) {
            parserData->currentFunc = st_add_id( &parserData->symTable[0], parserData->token.attribute.string->str );
            if (!&parserData->currentFunc) return ERR_INTERNAL;
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

    // <function definition> -> FUNCTION ID (<param list type 2>) : <param list type 3> <statement list> END
    } else if (parserData->token.attribute.keyword == KW_FUNCTION) {                            // FUNCTION
        
        if (res = get_next_token_and_check_type( parserData, T_IDE )) return res;               // ID
        
        parserData->currentFunc = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
        if (parserData->currentFunc == NULL || parserData->currentFunc->ifdef == 0) {
            parserData->currentFunc = st_add_id( &parserData->symTable[0], parserData->token.attribute.string->str );
            if (!&parserData->currentFunc) return ERR_INTERNAL;
        }
        else return ERR_SEMANTIC_UNDEF_VAR;

        parserData->currentFunc->ifdef = 1;
        parserData->currentFunc->ifdec = 1;
        
        // generate function header code
        cg_function_header( parserData->currentFunc->id );
        
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

    int res = 0;
    // if there is declared function or variable in the same context lvl with same identifier
    if (parserData->token.type == T_IDE && st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str ) != NULL
    && st_search( parserData->symTable[parserData->currentDepth].rootItem, parserData->token.attribute.string->str ) != NULL)
        return ERR_SEMANTIC_UNDEF_VAR;

    if (parserData->token.type == T_IDE || parserData->token.type == T_KEY) {
        // <param> -> TYPE, (in function header: case 1: input types; case 2: output types)
        if (parserData->whereAmI == 1 || parserData->whereAmI == 3) { 

            if (parserData->token.type == T_IDE) return ERR_SYNTAX;                            // TYPE

            if      (parserData->whereAmI == 1) {
                if (res = st_add_param( parserData->currentFunc->inputTypes, parserData->token.attribute.keyword )) return res;
                
            }
            else if (parserData->whereAmI == 3) {
                if (res = st_add_param( parserData->currentFunc->outputTypes, parserData->token.attribute.keyword )) return res;
                cg_function_output_type( parserData->token.attribute.keyword, parserData->currentFunc->outputTypes->length - 1 );
            }

            if (res = get_next_token_and_check_type( parserData, T_COM )) return res;          // ,    

        }
        // <param> -> ID : TYPE, (in function defenition)
        else if (parserData->whereAmI == 2) {
            
            if (parserData->token.type == T_KEY) return ERR_SYNTAX;                            // ID

            parserData->currentVar = st_add_id( &parserData->symTable[parserData->currentDepth], parserData->token.attribute.string->str );

            if (res = get_next_token_and_check_type( parserData, T_COL )) return res;          // :
            if (res = get_next_token( &parserData->token )) return res;                        // TYPE

            if (res = st_add_type( &parserData->token, parserData->currentVar )) return res;
            cg_function_input_type( parserData->currentFunc->id, parserData->token.attribute.keyword, parserData->currentFunc->inputTypes->length );

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

    int res = 0;
    // $1
    // <statement> -> <ID list> = <expression list>
    // <statement> -> <ID list> = ID (<param list>)
    // <statement> -> <ID list> <value list>
    // <statement> -> function_id(<params>)
    if (parserData->token.type == T_IDE) {

        // currentFunc <- search var id
        SEARCH_GLOBAL( parserData->token.attribute.string->str );

        // if not function : <statement> -> <ID list> <value list> 
        if (parserData->currentFunc == NULL) {
            // currentVar <- search var id
            SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
            if (parserData->currentVar == NULL) return ERR_SEMANTIC_UNDEF_VAR;

            if (res = get_next_token_and_apply_rule( parserData, "rule_IDList" )) return res;

        } 
        // <statement> -> function_ID (<param list>) 
        else if (parserData->currentFunc != NULL) {

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
        return rule_statementList( parserData );                                                        // <statement list>

    }

    // $2
    // <statement> -> LOCAL ID : TYPE <optional defenition>
    else if (parserData->token.attribute.keyword == KW_LOCAL) {                                         // LOCAL

        if (res = get_next_token_and_check_type( parserData, T_IDE )) return res;                       // ID

        SEARCH_GLOBAL( parserData->token.attribute.string->str );
        if (res != 0) exit( ERR_SEMANTIC_UNDEF_VAR );
        SEARCH_LOCAL( parserData->token.attribute.string->str );
        if (res != 0) exit( ERR_SEMANTIC_UNDEF_VAR );
        
        ADD_LOCAL( parserData->token.attribute.string->str );
        parserData->lhsId = parserData->currentVar;
        
        if (res = get_next_token_and_check_type( parserData, T_COL )) return res;                       // :
        if (res = get_next_token( &parserData->token )) return res;                                     // TYPE
        if (res = st_add_type( &parserData->token, parserData->currentFunc )) return res;
        printf( "(%d %d) ", parserData->token.type, parserData->token.attribute.keyword );
                                                                                                    
        if (res = get_next_token_and_apply_rule( parserData, "rule_optionalDefenition" )) return res;   // <optional defenition>
        // will give you next token

        return rule_statementList( parserData );                                                        // <statement list>

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

        parserData->currentFunc = st_search( parserData->symTable[0].rootItem, "write" );
        //st_print( parserData->symTable[0].rootItem );
        if (res = get_next_token_and_check_type( parserData, T_LBR )) return res;                   // (

        while (parserData->token.type != T_RBR) {

            // generate frame
            cg_frame_to_pass_param(  );

            if (res = get_next_token( &parserData->token )) return res;

            if (parserData->token.type == T_IDE) {

                for (int i = parserData->currentDepth; i > 0; i--) {
                    parserData->currentVar = st_search( parserData->symTable[i].rootItem, parserData->token.attribute.string->str);
                    if (parserData->currentVar != NULL) break;
                }
                if (parserData->currentVar == NULL) return ERR_SEMANTIC_UNDEF_VAR;

                cg_pass_param( &parserData->token, 1 );
                cg_call( "write" );

                if (res = get_next_token( &parserData->token )) return res;

                if (parserData->token.type == T_COM) {
                    // ADD_LINE( "WRITE string@\\010" );
                } else return ERR_SYNTAX;
                
                // ADD_LINE( "WRITE string@\\010" );

            } else if (parserData->token.type == T_STR) { // ' ' -> \032

                // ADD_CODE( "WRITE string@");
                // ADD_LINE( parserData->token.attribute.string->str );

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

/**
 * <ID list> -> ID <other_IDs>
 * <ID list> -> , ID <other IDs>
 */ 
int rule_IDList ( Parser_data *parserData ) {

    int res = 0;

    if (res = get_next_token_and_apply_rule( parserData, "rule_OtherIDs")) return res;



    return res;

}

/**
 * 
 * 
 */
int rule_OtherIDs ( Parser_data *parserData ) {

    int res = 0;



    return res;

}
/**
 * <optional defenition> -> = <value>
 * <optional defenition> -> e
 */
int rule_optionalDefenition ( Parser_data *parserData ) {

    int res = 0;
    // <optional defenition> -> e
    if (parserData->token.type != T_ASS) {

        return res;

    }
    // <optional defenition> -> = <value>
    else {

        if (res = get_next_token_and_apply_rule( parserData, "rule_Value" )) return res;

    }

    return res;

}
/**
 * 
 */
int rule_expressionList ( Parser_data *parserData ) {

    int res = 0;

    if (res = get_next_token( &parserData->token )) return res;
    return res;

}
/**
 * <value> -> function_ID( <argument list> )
 * <value> -> <expression>
 */
int rule_Value ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type == T_IDE || parserData->token.type == T_KEY) {

        // <value> -> ID
        SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
        if (res != 1) {      // not func

            if (res = rule_Expression( parserData )) return res;

            return res;

        } else if (IS_BUILTIN( parserData->token )) {

            switch (parserData->token.attribute.keyword) {

                case (KW_READI):

                    SEARCH_GLOBAL_RHS( "readi" );

                break;

                case (KW_READN):

                    SEARCH_GLOBAL_RHS( "readn" );

                break;

                case (KW_READS):

                    SEARCH_GLOBAL_RHS( "reads" );

                break;

                case (KW_WRITE):

                    exit( ERR_SEMANTIC_INCOP_TYPE );

                break;

                case (KW_TOINTEGER):

                    SEARCH_GLOBAL_RHS( "tointeger" );

                break;

                case (KW_SUBSTR):

                    SEARCH_GLOBAL_RHS( "substr" );

                break;

                case (KW_ORD):

                    SEARCH_GLOBAL_RHS( "ord" );

                break;

                case (KW_CHR):

                    SEARCH_GLOBAL_RHS( "chr" );

                break;

                default:

                    exit( ERR_SEMANTIC_UNDEF_VAR );

                break;

            } 

        } // if built-in

        else {

            SEARCH_GLOBAL_RHS( parserData->token.attribute.string->str );

        }

        if (parserData->rhsId) {

            int outType = parserData->rhsId->outputTypes->str[0] - '0';

            if (parserData->rhsId->outputTypes->length == 1 && 
            (parserData->lhsId->type == outType)) {

                cg_frame_to_pass_param(  );

                if (res = get_next_token_and_check_type( parserData, T_LBR )) return res;

                parserData->paramIndex = 0;
                if (res = get_next_token_and_apply_rule( parserData, "rule_ArgumentList")) return res;

            } else {

                exit( ERR_SEMANTIC_FUNC_PAR );

            }

        }

    }
    
}

/**
 * <argument list> -> 
 * 
 * 
 */
int rule_argumentList ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type == T_INT
    || parserData->token.type == T_NUM
    || parserData->token.type == T_STR
    || parserData->token.type == T_IDE) {

    }



}


#endif