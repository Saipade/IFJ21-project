////
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
    if      (!strcmp( rulename, "rule_paramList" )) rule_paramList( parserData );
    else if (!strcmp( rulename, "rule_type" )) rule_type( parserData );
    else if (!strcmp( rulename, "rule_functionBody")) rule_functionBody( parserData );

}




bool parser_data_init ( Parser_data *parserData ) {
    
    for (int i = 0; i < 10; i++) {
        if (!st_init( &parserData->symTable[i] )) return false;
    }
/*     st_init( &parserData->localTable );
    st_init( &parserData->globalTable ); */

    parserData->in_func = 0;
    parserData->if_decl = 0;
    parserData->in_decl = 0;
    parserData->in_loop = 0;

    parserData->lhsId = NULL;
    parserData->rhsId = NULL;
    parserData->currentId = NULL;

    parserData->labelId = 0;
    parserData->paramId = 0;
    parserData->labelDepth = 0;
    parserData->whereAmI = 0;

    return true;

}



int parse (  ) {

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

    int res;
    // general rule: 
    // <program> -> <prologue>.<function list>
    // <prologue>
    if (res = rule_prologue( parserData )) return res;
    // <function list>
    if (res = rule_functionDeclaration( parserData )) return res;

    printf( "%d\n", res );

    return res; 

}

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
 */ 
int rule_functionList (Parser_data *parserData ) {

    int res;
    // <function declaration> -> GLOBAL ID : FUNCTION (<param list type 1>) : <param list type 3> . <function list>
    if (parserData->token.attribute.keyword == KW_GLOBAL) {

        if (res = get_next_token_and_check_type( parserData, TT_IDE )) return res;

        parserData->currentId = st_add_id( &parserData->symTable[0], parserData->token.attribute.string->str );

        if (res = get_next_token_and_check_type( parserData, TT_COL )) return res;
        if (res = get_next_token_and_check_keyword( parserData, KW_FUNCTION )) return res;
        if (res = get_next_token_and_check_type( parserData, TT_LBR )) return res;
        
        parserData->whereAmI = 1;
        if (res = get_next_token_and_apply_rule( parserData, "rule_paramList" )) return res;
        // rule_paramList gives you next token
        printf( "(%d -)\n", parserData->token.type );
        if (parserData->token.type != TT_RBR) return ERR_SYNTAX;
        if (res = get_next_token_and_check_type( parserData, TT_COL )) return res;

        parserData->whereAmI = 3;
        if (res = get_next_token_and_apply_rule( parserData, "rule_paramList" )) return res;
        // rule_paramList gives you next token
        return rule_functionDeclaration( parserData );

    // <function declaration> -> FUNCTION ID (<param list type 2>) : <param list type 3> . <function list>
    } else if (parserData->token.attribute.keyword == KW_FUNCTION) {

        if (res = get_next_token_and_check_type( parserData, TT_IDE )) return res;

        parserData->currentId = st_add_id( &parserData->symTable[0], parserData->token.attribute.string->str );
        if (res = get_next_token_and_check_type( parserData, TT_LBR )) return res;

        parserData->whereAmI = 2;

    }

    else if (parserData->token.type == TT_EOL) {

       if (res = get_next_token( &parserData->token )) return res;
       return rule_functionDeclaration( parserData );

    }

    else {
        
        return 0;

    }

}

/**
 * <param list> -> <param>.<param list>
 * <param list> -> <param>
 * <param> -> DATA_TYPE (function declaration or function's output types)
 * <param> -> ID : DATA_TYPE (function defenition)
 */ 
int rule_paramList ( Parser_data *parserData ) {

    int res;

    if (parserData->token.type == TT_IDE || parserData->token.type == TT_KEY) {
        // you're in function declaration
        if (parserData->whereAmI == 1) { 

            if (parserData->token.type == TT_IDE) return ERR_SYNTAX;
            if (res = st_add_param( parserData )) return res;

        }
        // you're in function defenition
        else if (parserData->whereAmI == 2) {
            
            if (parserData->token.type == TT_IDE) 
                if (res = st_add_id( &parserData->symTable[1], parserData->token.attribute.string->str )) return res;
            if (parserData->token.type == TT_KEY) 
                if (res = st_add_param( parserData )) return res;

        }
        // you're in function's output
        else if (parserData->whereAmI == 3) {
            
            if (parserData->token.type == TT_IDE) return ERR_SYNTAX;
            if (!ds_add_next( parserData->currentId->types, '#' )) return ERR_INTERNAL;
            if (res = st_add_param( parserData )) return res;

        }

        if (res = get_next_token( &parserData->token )) return res;
        return rule_functionDeclaration( parserData ); 

    }

    else {

        return 0;

    }

}
/**
 * Definition of functions
 * <function list> -> <function>.<function list>
 * <function list> -> <function>
 * <function> -> FUNCTION ID.(<param list>) : <return types list>.<function body>.END
 * 
 */
/* int rule_functionList ( Parser_data *parserData ) {

    int res;

    if (parserData->token.attribute.keyword == KW_FUNCTION) {

        if (res = get_next_token_and_check_type( parserData, TT_IDE )) return res;
        parserData->currentId = st_add_id( &parserData->symTable[0], parserData->token.attribute.string->str );

        if (res = get_next_token_and_check_type( parserData, TT_LBR )) return res;

        parserData->whereAmI = 2;
        if (res = get_next_token_and_apply_rule( parserData, "rule_paramList" )) return res; 

        if (res = get_next_token_and_check_type( parserData, TT_RBR )) return res;
        if (res = get_next_token_and_check_type( parserData, TT_COL )) return res;

        parserData->whereAmI = 3;
        if (res = get_next_token_and_apply_rule( parserData, "rule_paramList" )) return res;

        if (res = get_next_token_and_apply_rule( parserData, "rule_functionBody" )) return res;

        if (res = get_next_token_and_check_keyword ( parserData, KW_END )) return res;

        return rule_functionList( parserData );
    }

    else {

        return 0;

    }

} */

/**
 * <function body> -> <statement sequence>
 * <function body> -> ε
 * <statement sequence> -> <statement>.<statement sequence>
 * <statement> -> ... (viz function if cases)
 * <statement> -> ε
 */
int rule_functionBody ( Parser_data *parserData ) {

    int res;
    // ID, ID, ..., ID = EXPR, EXPR, ..., EXPR
    if (parserData->token.type == TT_IDE) {



    }
    // <statement> -> LOCAL ID : TYPE = INT/NUM/STR_value
    if (parserData->token.attribute.keyword == KW_LOCAL) {

        if (res = get_next_token_and_check_type( parserData, TT_IDE )) return res;
        parserData->currentId = st_add_id( &parserData->symTable[parserData->currentDepth], parserData->token.attribute.string->str );

        if (res = get_next_token_and_check_type( parserData, TT_COL )) return res;
        
        if (res = get_next_token_and_check_type( parserData, TT_KEY )) return res;
        if (res = st_add_type( parserData )) return res;

        if (res = get_next_token_and_check_type( parserData, TT_EQU )) return res;
        if (res = st_add_value( parserData )) return res;

        return rule_functionBody( parserData );

    }
    // <statement> -> IF <condition> THEN <statement sequence> ELSE <statement sequence> END
    if (parserData->token.attribute.keyword == KW_IF) {
        
        parserData->currentDepth++;
        if (res = get_next_token_and_apply_rule( parserData, "rule_condition" )) return res;
        if (res = get_next_token_and_check_keyword( parserData, KW_THEN )) return res;
        if (res = get_next_token_and_apply_rule( parserData, "rule_functionBody" )) return res;
        if (res = get_next_token_and_check_keyword( parserData, KW_ELSE )) return res;
        if (res = get_next_token_and_apply_rule( parserData, "rule_functionBody" )) return res;
        if (res = get_next_token_and_check_keyword( parserData, KW_END )) return res;
        parserData->currentDepth--;
        return rule_functionBody( parserData );

    }

    // <statement> -> WhILE <condition> DO <statement sequence> END
    if (parserData->token.attribute.keyword == KW_WHILE) {


    }
}


#endif
