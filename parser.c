/**
 * PROJECT: Implementation of imperative programming language translator (IFJ21)
 * PART: Recursive top-down parser
 *
 * AUTHOR(S): Maksim Tikhonov (xtikho00) 
 */

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

void q_init ( Queue *queue ) {

    queue->top = NULL;
    queue->end = NULL;

}

void q_push ( Queue *queue, Item_data *item ) {

    Queue_item *newItem = malloc( sizeof ( Queue_item ) );
    if (!newItem) return;

    newItem->data = item;
    newItem->next = NULL;

    if (queue->top == NULL) queue->top = newItem;
    else queue->end->next = newItem;
    queue->end = newItem;

}

Item_data *q_pop ( Queue *queue ) {

    Item_data *returnData;

    if (queue->top) {

        Item_data *returnData = queue->top->data;
        Queue_item *itemToDelete = queue->top;
        if (queue->top == queue->end) {
            queue->end = NULL;
        }

        queue->top = queue->top->next;
        free( itemToDelete );

        return returnData;

    }

    return NULL;

}

void q_dispose( Queue *queue ) {

    while (queue->top) {

        q_pop( queue );

    }

}

Data_type get_param_or_retval_type ( Item_data *function, bool mode, int pos ) {

    int res = 0;

    if (!mode) res = function->inputTypes->str[pos];
    else res = function->outputTypes->str[pos];

    switch (res) {

        case ('i'):
            return T_INT;
        case ('n'):
            return T_NUM;
        case ('s'):
            return T_STR;
        default:
            return T_NDA; 

    }
    
}

char convert_data_type_to_char ( Keyword dataType ) {

    switch (dataType) {

        case KW_INTEGER:
            return 'i';
        case KW_NUMBER:
            return 'n';
        case KW_STRING:
            return 's';
        default:
            return ERR_SYNTAX;

    }
    
}

int get_next_token_and_check_type ( Parser_data *parserData, Data_type type ) {
    
    int tmp;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    printf( "(%d -) ", parserData->token.type );
    if (parserData->token.type != type) return ERR_SYNTAX;
    
    return 0;

}

int get_next_token_and_check_keyword ( Parser_data *parserData, Keyword keyword ) {

    int tmp;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    printf( "(%d %d) ", parserData->token.type, parserData->token.attribute.keyword );
    if (parserData->token.type != T_KEY
        || parserData->token.attribute.keyword != keyword) return ERR_SYNTAX;
    
    return 0;

}

int get_next_token_and_apply_rule ( Parser_data *parserData, char *rulename ) {
    
    int tmp;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    printf( "(%d -) ", parserData->token.type );

         if (!strcmp( rulename, "rule_expression" ))            { if (tmp = rule_expression( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_statementList"))          { if (tmp = rule_statementList( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_optionalDefinition"))     { if (tmp = rule_optionalDefinition( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_declarationParams" ))     { if (tmp = rule_declarationParams( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_output" ))                { if (tmp = rule_output( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_definitionParams" ))      { if (tmp = rule_definitionParams( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_definitionParam" ))       { if (tmp = rule_definitionParam( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_value"))                  { if (tmp = rule_value( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_IDList" ))                { if (tmp = rule_IDList( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_otherID" ))               { if (tmp = rule_otherID( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_assignmentValues"))       { if (tmp = rule_assignmentValues( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_otherAssignmentValue" ))  { if (tmp = rule_otherAssignmentValue( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_functionCallParams" ))    { if (tmp = rule_functionCallParams( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_callParam" ))             { if (tmp = rule_callParam( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_assignmentValue" ))       { if (tmp = rule_assignmentValue( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_returnList" ))            { if (tmp = rule_returnList( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_returnValue" ))           { if (tmp = rule_returnValue( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_writeArgumentList" ))     { if (tmp = rule_writeArgumentList( parserData )) return tmp; }
    else if (!strcmp( rulename, "rule_otherWriteArgument" ))    { if (tmp = rule_otherWriteArgument( parserData )) return tmp; }
    else { printf("RULE DOES NOT EXIST\n"); return 0; }

}

bool parser_data_init ( Parser_data *parserData ) {

    int res = 0;

    // parser data parameters

    parserData->inLoop = 0;

    parserData->lhsId = NULL;
    parserData->rhsId = NULL;
    parserData->currentFunc = NULL;
    parserData->currentVar = NULL;
    parserData->insideFunc = NULL;
    
    parserData->paramIndex = 0;
    parserData->currentDepth = 0;
    parserData->loopCount = 0;
    parserData->expType = T_NDA;

    // symTable[0] - global table; all others are local ones
    st_init( &parserData->symTable[0] );
    q_init( &parserData->queue );
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
    if (res = st_add_param( tmp->inputTypes, ANY )) return false;
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
    // <prologue>
    if (res = rule_prologue( parserData )) return res;
    // <function list>
    if (res = rule_functionList( parserData )) return res;
    
    q_dispose( &parserData->queue );
    st_dispose( &parserData->symTable[0].rootItem );
    
    cg_output(stdout);
    ds_free( scannerString );

    return res; 

}

/*........................................ RECURSIVE TOP-DOWN PARSER ........................................ */

int rule_prologue ( Parser_data *parserData ) {
    
    int tmp;
    if (tmp = get_next_token_and_check_keyword ( parserData, KW_REQUIRE )) exit( tmp );
    if (tmp = get_next_token( &parserData->token )) return tmp;
    if (strcmp( parserData->token.attribute.string->str, "ifj21" ) != 0) exit( ERR_SYNTAX );
    printf( "(%d %s) ", parserData->token.type, parserData->token.attribute.string->str );
    // prologue OK -> generate start of program 
    if (!cg_start(  )) return ERR_INTERNAL;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    return 0;

}

/*........................................ FUNCTION HEADERS ........................................ */

int rule_functionList ( Parser_data *parserData ) {

    int res = 0;
    
    parserData->currentFunc = NULL;
    parserData->currentVar = NULL;
    parserData->lhsId = NULL;
    parserData->rhsId = NULL;
    parserData->insideFunc = NULL;

    printf( "(%d %d) ", parserData->token.type, parserData->token.attribute.keyword );
    if      (parserData->token.type == T_EOF) {

        return 0;

    } 

    else {

        if      (parserData->token.attribute.keyword == KW_GLOBAL) {

            if (res = rule_functionDeclaration( parserData )) exit( res );

            return rule_functionList( parserData );

        }

        else if (parserData->token.attribute.keyword == KW_FUNCTION) {
            
            if (res = rule_functionDefinition( parserData )) exit( res );

            return rule_functionList( parserData );

        }

        else if (parserData->token.type == T_IDE) {

            if (res = rule_entryPoint( parserData )) exit( res );

            return rule_functionList( parserData );

        }

    }

}

int rule_entryPoint ( Parser_data *parserData ) {

    int res = 0;                                                                                            // ID
    
    SEARCH_GLOBAL( parserData->token.attribute.string->str );
    if (res == 0) exit( ERR_SEMANTIC_UNDEF_VAR );
    res = 0;
    // called function must have no input and no output
    if (parserData->currentFunc->inputTypes->length != 0 || parserData->currentFunc->outputTypes->length != 0) exit( ERR_SEMANTIC_OTHER );


    if (res = get_next_token_and_check_type( parserData, T_LBR )) exit( res );                              // (
    if (res = get_next_token_and_check_type( parserData, T_RBR )) exit( res );                              // )

    cg_call( parserData->currentFunc->id );

    if (res = get_next_token( &parserData->token )) exit( res );
    return 0;

}

int rule_functionDeclaration ( Parser_data *parserData ) {

    int res = 0;                                                                                            // GLOBAL

    if (res = get_next_token_and_check_type( parserData, T_IDE )) exit( res );                              // ID
    SEARCH_GLOBAL( parserData->token.attribute.string->str );
    if (res == 1) exit( ERR_SEMANTIC_UNDEF_VAR ); // attempt of function redeclaration or declaration after definition
    ADD_GLOBAL( parserData->token.attribute.string->str );

    if (res = get_next_token_and_check_type( parserData, T_COL )) exit ( res );                             // :
    if (res = get_next_token_and_check_keyword( parserData, KW_FUNCTION )) exit ( res );                    // FUNCTION
    if (res = get_next_token_and_check_type( parserData, T_LBR )) exit ( res );                             // (

    if (res = get_next_token_and_apply_rule( parserData, "rule_declarationParams" )) exit( res );           // <declaration params>
    if (parserData->token.type != T_RBR) exit( ERR_SYNTAX );                                                // )

    if (res = get_next_token_and_apply_rule( parserData, "rule_output" )) exit( res );                      // <output>
    
    parserData->currentFunc->ifdec = 1;

    return 0;

}

int rule_declarationParams ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type == T_RBR) return res;

    if (res = st_add_param( parserData->currentFunc->inputTypes, parserData->token.attribute.keyword )) exit( res );

    return rule_otherDeclarationParam( parserData );

}

int rule_otherDeclarationParam ( Parser_data *parserData ) {

    int res = 0;
    
    if (res = get_next_token( &parserData->token )) exit( res );

    if (parserData->token.type != T_COM) return res;

    if (res = get_next_token( &parserData->token )) exit( res );
    if (res = st_add_param( parserData->currentFunc->inputTypes, parserData->token.attribute.keyword )) exit( res );

    return rule_otherDeclarationParam( parserData );

}

int rule_functionDefinition ( Parser_data *parserData ) {

    int res = 0;                                                                                            // FUNCTION
    
    if (res = get_next_token_and_check_type( parserData, T_IDE )) exit( res );                              // ID   
    parserData->currentFunc = st_search( parserData->symTable[0].rootItem, parserData->token.attribute.string->str );
    if (parserData->currentFunc == NULL) {
        ADD_GLOBAL( parserData->token.attribute.string->str );
        parserData->currentFunc->ifdef = 1;
    }
    else { // attempt of function redefinition?
        if (parserData->currentFunc->ifdef == 1) exit ( ERR_SEMANTIC_UNDEF_VAR ); 
        else parserData->currentFunc->ifdef = 1;
    }

    parserData->insideFunc = parserData->currentFunc;
    cg_function_header( parserData->insideFunc->id );

    if (res = get_next_token_and_check_type( parserData, T_LBR )) exit( res );                              // (
    
    parserData->currentDepth++;
    st_init( &parserData->symTable[parserData->currentDepth] );
    
    if (res = get_next_token_and_apply_rule( parserData, "rule_definitionParams" )) exit( res );            // <definition params>
    
    if (parserData->token.type != T_RBR) exit( ERR_SYNTAX );                                                // )
    
    if (res = get_next_token_and_apply_rule( parserData, "rule_output" )) exit( res );                      // <output>

    if (res = rule_statementList( parserData )) exit( res );                                                // <statement list>

    st_dispose( &parserData->symTable[parserData->currentDepth].rootItem );
    parserData->currentDepth--;
    
    if (parserData->token.attribute.keyword != KW_END) exit( ERR_SYNTAX );                                  // END

    cg_function_return( parserData->insideFunc->id );

    if (res = get_next_token( &parserData->token )) exit( res );
    return 0;
    
}

int rule_definitionParams ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type == T_RBR) {
        if (parserData->currentFunc->inputTypes->length == 0) return res;
        else exit( ERR_SEMANTIC_FUNC_PAR );
    }
    parserData->paramIndex = 0;

    if (res = rule_definitionParam( parserData )) exit( res );

    return rule_otherDefinitionParam( parserData );

}

int rule_otherDefinitionParam ( Parser_data *parserData ) {

    int res = 0;

    if (res = get_next_token( &parserData->token )) exit( res );                                            // ,
    printf("(%d -) ", parserData->token.type );
    
    if (parserData->token.type != T_COM) {
        if (parserData->currentFunc->ifdec) {
            if (parserData->currentFunc->inputTypes->length == parserData->paramIndex) return res;
            else exit( ERR_SEMANTIC_FUNC_PAR );
        } else return res;
    }

    if (res = get_next_token_and_apply_rule( parserData, "rule_definitionParam" )) return res;              // <definition param>
    
    return rule_otherDefinitionParam( parserData );                                                         // <other definition param>

}

int rule_definitionParam ( Parser_data *parserData ) {

    int res = 0;
    
    ADD_LOCAL( parserData->token.attribute.string->str );                                                   // ID
    
    if (res = get_next_token_and_check_type( parserData, T_COL )) exit( res );                              // :
    if (res = get_next_token( &parserData->token )) exit( res );                                            // TYPE
    printf("(%d %d) ", parserData->token.type, parserData->token.attribute.keyword );
    // to control if types are the same both in declared and in defined versions
    if (parserData->currentFunc->ifdec == 1) {

        if (parserData->currentFunc->inputTypes->length == parserData->paramIndex) exit( ERR_SEMANTIC_FUNC_PAR );
        if (parserData->currentFunc->inputTypes->str[parserData->paramIndex] != convert_data_type_to_char( parserData->token.attribute.keyword )) {
            exit( ERR_SEMANTIC_FUNC_PAR );
        }
        
    } 
    // to add types to function
    else {
        
        if (res = st_add_param( parserData->currentFunc->inputTypes, parserData->token.attribute.keyword )) exit( res );

    }
    
    // assign type to variable
    if (res = st_add_type( &parserData->token, parserData->currentVar )) exit( res );

    if (parserData->currentFunc->ifdef) cg_function_param( parserData );

    parserData->paramIndex++;

    return 0;

}

int rule_output ( Parser_data *parserData ) {
    
    int res = 0;

    if (parserData->token.type != T_COL) {
        if (parserData->currentFunc->ifdec) {
            if (parserData->currentFunc->outputTypes->length == 0) return res;
            else exit( ERR_SEMANTIC_FUNC_PAR );
        } else return res;
    }

    parserData->paramIndex = 0;
    if (res = get_next_token( &parserData->token )) exit( res );                                            // TYPE
    printf("(%d %d) ", parserData->token.type, parserData->token.attribute.keyword );
    if (parserData->currentFunc->ifdec) {
        
        if (parserData->currentFunc->outputTypes->str[parserData->paramIndex] != convert_data_type_to_char( parserData->token.attribute.keyword )) exit( ERR_SEMANTIC_FUNC_PAR );

    }
    else {

        if (res = st_add_param( parserData->currentFunc->outputTypes, parserData->token.attribute.keyword)) exit( res );

    }
    
    cg_function_retval( parserData->paramIndex );
    parserData->paramIndex++;

    if (res = get_next_token( &parserData->token )) exit( res );
    printf("(%d -) ", parserData->token.type );
    return rule_otherOutputType( parserData );

}

int rule_otherOutputType ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type != T_COM) {
        if (parserData->currentFunc->ifdec) {
            if (parserData->paramIndex == parserData->currentFunc->outputTypes->length) return res;
            else exit( ERR_SEMANTIC_FUNC_PAR );
        } else return res;
    }

    if (res = get_next_token( &parserData->token )) exit( res );                                            // TYPE
    printf("(%d %d) ", parserData->token.type, parserData->token.attribute.keyword );
    if (parserData->currentFunc->ifdec) {
        
        if (parserData->currentFunc->outputTypes->str[parserData->paramIndex] != convert_data_type_to_char( parserData->token.attribute.keyword )) exit( ERR_SEMANTIC_FUNC_PAR );
        
    }
    else {

        if (res = st_add_param( parserData->currentFunc->outputTypes, parserData->token.attribute.keyword)) exit( res );

    }

    cg_function_retval( parserData->paramIndex );
    parserData->paramIndex++;

    if (res = get_next_token( &parserData->token )) exit( res );
    return rule_otherOutputType( parserData );
    
}

/*.......................................... STATEMENT LIST .......................................... */

int rule_statementList ( Parser_data *parserData ) {
    
    int res = 0;

    // $1
    // <statement> -> <ID list> <assignment values>
    // <statement> -> ID (<function call param list>)
    if (parserData->token.type == T_IDE) {
        
        // currentFunc <- search var id 
        SEARCH_GLOBAL( parserData->token.attribute.string->str );

        // <statement> -> <ID list> <smth>
        if (res == 0) {

            if (res = rule_IDList( parserData )) exit( res );                                               // <ID list>
            if (res = rule_assignmentValues( parserData )) exit( res );                                     // <assignment values>

        } 
        // <statement> -> function_ID (<function call param list>) 
        else if (res == 1) {
            
            res = 0;
            
            if (!cg_frame_to_pass_param(  )) exit( ERR_INTERNAL );
            if (res = get_next_token_and_check_type( parserData, T_LBR )) exit( res );                      // (
            if (res = get_next_token_and_apply_rule( parserData, "rule_functionCallParams" )) exit( res );  // <function call param list>
            if (parserData->token.type != T_RBR) exit( ERR_SYNTAX );                                        // )
            if (!cg_call( parserData->currentFunc->id )) exit( ERR_INTERNAL );      

        }
        if (parserData->token.type == T_RBR) {
            if (res = get_next_token( &parserData->token )) exit( res );
        }
        return rule_statementList( parserData );

    }

    // $2
    // <statement> -> LOCAL ID : TYPE <optional definition>
    else if (parserData->token.attribute.keyword == KW_LOCAL) {                                             // LOCAL
        
        printf( "(%d %d) ", parserData->token.type, parserData->token.attribute.keyword );
        if (res = get_next_token_and_check_type( parserData, T_IDE )) return res;                           // ID

        SEARCH_GLOBAL( parserData->token.attribute.string->str ); // variable with function name -> error
        if (res == 1) exit( ERR_SEMANTIC_UNDEF_VAR );
        SEARCH_LOCAL( parserData->token.attribute.string->str ); // attempt of variable redefinition on the same lvl
        if (res == 1) exit( ERR_SEMANTIC_UNDEF_VAR );

        ADD_LOCAL( parserData->token.attribute.string->str );
        parserData->lhsId = parserData->currentVar;
        
        if (res = get_next_token_and_check_type( parserData, T_COL )) return res;                           // :
        if (res = get_next_token( &parserData->token )) return res;                                         // TYPE
        printf( "(%d %d) ", parserData->token.type, parserData->token.attribute.keyword );
        if (res = st_add_type( &parserData->token, parserData->lhsId )) return res;

        cg_declare_var( parserData );
        cg_define_var( parserData );
        
        if (res = get_next_token_and_apply_rule( parserData, "rule_optionalDefinition" )) return res;       // <optional definition>
        
        return rule_statementList( parserData );                                                            // <statement list>

    }

    // $3
    // <statement> -> IF <expression> THEN <statement list> ELSE <statement list> END <statement list>
    else if (parserData->token.attribute.keyword == KW_IF) {                                                // IF
        
        parserData->currentDepth++;
        st_init( &parserData->symTable[parserData->currentDepth] );


        if (res = get_next_token_and_apply_rule( parserData, "rule_expression" )) return res;               // <expression>
        if (res = get_next_token_and_check_keyword( parserData, KW_THEN )) return res;                      // THEN
        if (res = get_next_token_and_apply_rule( parserData, "rule_statementList" )) return res;            // <statement list>
        if (res = get_next_token_and_check_keyword( parserData, KW_ELSE )) return res;                      // ELSE
        if (res = get_next_token_and_apply_rule( parserData, "rule_statementList" )) return res;            // <statement list>
        if (res = get_next_token_and_check_keyword( parserData, KW_END )) return res;                       // END

        st_dispose( &parserData->symTable[parserData->currentDepth].rootItem );
        parserData->currentDepth--;

        if (res = get_next_token( &parserData->token )) return res;
        return rule_statementList( parserData );                                                            // <statement list>

    }

    // $4
    // <statement> -> WhILE <expression> DO <statement list> END . <statement list>
    else if (parserData->token.attribute.keyword == KW_WHILE) {                                             // WHILE
        
        parserData->currentDepth++;
        st_init( &parserData->symTable[parserData->currentDepth] );
        parserData->inLoop = true;
        
        if (res = get_next_token_and_apply_rule( parserData, "rule_expression" )) return res;               // <expression>
        if (res = get_next_token_and_check_keyword( parserData, KW_DO )) return res;                        // DO
        if (res = get_next_token_and_apply_rule( parserData, "rule_statementList" )) return res;            // <statement list>
        if (res = get_next_token_and_check_keyword( parserData, KW_END )) return res;                       // END

        parserData->inLoop = false;
        st_dispose( &parserData->symTable[parserData->currentDepth].rootItem );
        parserData->currentDepth--;

        if (res = get_next_token( &parserData->token )) return res;
        return rule_statementList( parserData );                                                            // <statement list>
        
    }

    // $5
    // <statement> -> RETURN <expression list>
    else if (parserData->token.attribute.keyword == KW_RETURN) {
        printf("(%d %d) ", parserData->token.type, parserData->token.attribute.keyword);
        if (res = get_next_token_and_apply_rule( parserData, "rule_returnList" )) exit( res );
        
        return rule_statementList( parserData );                                                            // <statement list>

    }

    // $6
    // <statement> -> WRITE (<write argument list>)
    else if (parserData->token.attribute.keyword == KW_WRITE) {                                             // WRITE

        printf( "(%d %d) ", parserData->token.type, parserData->token.attribute.keyword );
        SEARCH_GLOBAL( "write" );
        
        if (res = get_next_token_and_check_type( parserData, T_LBR )) exit( res );                          // (

        if (res = get_next_token_and_apply_rule( parserData, "rule_writeArgumentList" )) return res;        // <write argument list>
        
        if (parserData->token.type != T_RBR) exit( ERR_SYNTAX );                                            // ) 
        
        if (res = get_next_token( &parserData->token )) exit( res );
        return rule_statementList( parserData );
        
    }


    // $
    // <statement> -> ε
    else return 0;

}

/*.......................................... IDENTIFIER LIST ..........................................*/

int rule_IDList ( Parser_data *parserData ) {

    int res = 0;                                                                                            // ID
    
    SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
    if (res == 0) exit( ERR_SEMANTIC_UNDEF_VAR );
    res = 0;
    q_push( &parserData->queue, parserData->currentVar );

    if (res = get_next_token( &parserData->token )) exit( res );

    return rule_otherID( parserData );                                                                      // <other ID>

}

int rule_otherID ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type != T_COM) return res;                                                        // ,
    if (res = get_next_token_and_check_type( parserData, T_IDE )) exit( res );                              // ID
    SEARCH_GLOBAL( parserData->token.attribute.string->str );
    if (res == 1) exit( ERR_SEMANTIC_OTHER );
    SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
    if (res == 0) exit( ERR_SEMANTIC_UNDEF_VAR );
    res = 0;
    q_push( &parserData->queue, parserData->currentVar );
   
    if (res = get_next_token( &parserData->token )) exit( res );
    return rule_otherID( parserData );                                                                      // <other ID>

}

/*.......................................... ASSIGNMENT VALUES ..........................................*/

int rule_assignmentValues( Parser_data *parserData ) {

    int res = 0;
    
    if (parserData->token.type != T_ASS) exit( ERR_SYNTAX );                                                // =
    if (res = get_next_token_and_apply_rule( parserData, "rule_assignmentValue" )) exit( res );             // <assignment value>
    return rule_otherAssignmentValue( parserData );                                                         // <other assignment value>

}

int rule_otherAssignmentValue( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type != T_COM) return res;                                                        // ,
    if (res = get_next_token_and_apply_rule( parserData, "rule_assignmentValue")) exit( res );              // <assignment value>
    return rule_otherAssignmentValue( parserData );                                                         // <other assignment value>

}

int rule_assignmentValue ( Parser_data *parserData ) {

    int res = 0;
    // value ? variable : function : constant
    if (parserData->token.type == T_IDE) {

        SEARCH_GLOBAL( parserData->token.attribute.string->str );
        // function
        if (res == 0) { // variable

            parserData->lhsId = q_pop( &parserData->queue );
            if (res = rule_expression( parserData )) exit( res );

        }
        
        else if (res == 1) { 

            res = 0;

            if (!cg_frame_to_pass_param(  )) exit( ERR_INTERNAL );
            if (res = get_next_token_and_check_type( parserData, T_LBR )) exit( res );                          // (
            if (res = get_next_token_and_apply_rule( parserData, "rule_functionCallParams" )) exit( res );      // <function call params>
            if (parserData->token.type != T_RBR) exit( ERR_SYNTAX );                                            // )
            if (!cg_call( parserData->currentFunc->id )) exit( ERR_INTERNAL );                 
            for (int i = 0; i < parserData->currentFunc->outputTypes->length; i++) {                            // value return

                parserData->currentVar = q_pop( &parserData->queue );
                if (parserData->currentVar->type != get_param_or_retval_type( parserData->currentFunc, 1, i )) exit( ERR_SEMANTIC_FUNC_PAR );
                if (!cg_get_retval( parserData->currentVar, i )) exit( ERR_INTERNAL );

            }

        }

    } 
    // constant
    else if (IS_VAL( parserData->token )) { 

        parserData->lhsId = q_pop( &parserData->queue );
        if (res = rule_expression( parserData )) exit( res );

    } else exit( ERR_SYNTAX );

    return 0;

}

/*.......................................... FUNCTION CALL ..........................................*/

int rule_functionCallParams( Parser_data *parserData ) {

    int res = 0;

    parserData->paramIndex = 0;
    if (res = rule_callParam( parserData )) exit( res );                                                    // <call param>  
    if (res = get_next_token( &parserData->token )) exit( res );

    return rule_otherCallParam( parserData );                                                               // <other call param>
    
}

int rule_otherCallParam ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type != T_COM) return res;                                                        // ,

    if (res = get_next_token_and_apply_rule( parserData, "rule_callParam" )) exit( res );                   // <call param>
    if (res = get_next_token( &parserData->token )) exit( res );
    
    return rule_otherCallParam( parserData );                                                               // <other call param>

}

int rule_callParam ( Parser_data *parserData ) {
    
    int res = 0;

    if (parserData->currentFunc->inputTypes->length == parserData->paramIndex) exit( ERR_SEMANTIC_FUNC_PAR );

    if (parserData->token.type == T_IDE) {
        // parserData->currentVar <- search
        SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
        if (res == 0) exit( ERR_SEMANTIC_UNDEF_VAR );
        res = 0;
        
        if (parserData->currentVar->type != get_param_or_retval_type( parserData->currentFunc, 0, parserData->paramIndex )) exit( ERR_SEMANTIC_FUNC_PAR );

        if (!cg_pass_param_light( parserData )) exit( ERR_INTERNAL );
        
    }
    
    else if (IS_VAL( parserData->token )) {

        if (!cg_pass_param_light( parserData )) exit( ERR_INTERNAL );

    }

    else exit( ERR_SYNTAX );

    parserData->paramIndex++;
    return 0;

}

int rule_optionalDefinition ( Parser_data *parserData ) {

    int res = 0;
    // <optional definition> -> e
    if (parserData->token.type != T_ASS) {

        return res;

    }
    // <optional definition> -> = <value>
    else {

        if (res = get_next_token_and_apply_rule( parserData, "rule_value" )) return res;
        
    }

    return res;

}

int rule_value ( Parser_data *parserData ) {
    
    int res = 0;
    
    if ((parserData->token.type == T_IDE || parserData->token.type == T_KEY) || IS_VAL( parserData->token )) {
        
        if (IS_VAL( parserData->token )) {

            if (res = rule_expression( parserData )) exit( res );
            
            return res;    

        }
        // <value> -> ID
        if (parserData->token.type != T_KEY) SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
        if (res == 1) {      // not func
        
            res = 0;

            if (res = rule_expression( parserData )) return res;

            return res;
            
        } else if (IS_BUILTIN( parserData->token )) {
            
            switch (parserData->token.attribute.keyword) {

                case (KW_READI):
                    SEARCH_GLOBAL( "readi" );
                break;

                case (KW_READN):
                    SEARCH_GLOBAL( "readn" );
                break;

                case (KW_READS):
                    SEARCH_GLOBAL( "reads" );
                break;

                case (KW_WRITE):
                    exit( ERR_SEMANTIC_INCOP_TYPE );
                break;

                case (KW_TOINTEGER):
                    SEARCH_GLOBAL( "tointeger" );
                break;

                case (KW_SUBSTR):
                    SEARCH_GLOBAL( "substr" );
                break;

                case (KW_ORD):
                    SEARCH_GLOBAL( "ord" );
                break;

                case (KW_CHR):
                    SEARCH_GLOBAL( "chr" );
                break;

                default:

                    exit( ERR_SEMANTIC_UNDEF_VAR );

                break;

            } 

        } // if built-in

        else { // if not

            SEARCH_GLOBAL( parserData->token.attribute.string->str );

        }

        if (parserData->currentFunc) {

            res = 0;

            if (!cg_frame_to_pass_param(  )) exit( ERR_INTERNAL );
            if (res = get_next_token_and_check_type( parserData, T_LBR )) exit( res );                          // (
            if (res = get_next_token_and_apply_rule( parserData, "rule_functionCallParams" )) exit( res );      // <function call params>
            if (parserData->token.type != T_RBR) exit( ERR_SYNTAX );                                            // )
            if (!cg_call( parserData->currentFunc->id )) exit( ERR_INTERNAL );
            Data_type type = get_param_or_retval_type( parserData->currentFunc, 1, 0 );
            if (parserData->lhsId->type != type) {
                if      (parserData->lhsId->type == T_NUM && type == T_INT) {
                    if (!cg_convert_res_int2num( m_ret )) exit( ERR_INTERNAL );
                } 
                else if (parserData->lhsId->tyfpe == T_INT && type == T_NUM) {
                    if (!cg_convert_res_num2int( m_ret )) exit( ERR_INTERNAL );
                } 
                else exit( ERR_SEMANTIC_FUNC_PAR );
            }
            if (!cg_get_retval( parserData->lhsId, 0 )) exit( ERR_INTERNAL );

            if (res = get_next_token( &parserData->token )) exit( res );
            return 0;

        } else exit( ERR_SEMANTIC_UNDEF_VAR );

    }
    
}

/*.......................................... RETURN ..........................................*/

int rule_returnList ( Parser_data *parserData ) {
    
    int res = 0;
    
    if ((!IS_VAL( parserData->token )) && parserData->token.type != T_IDE) {                                  // e
        cg_jump_to_end( parserData->insideFunc->id );
        return res;
    }
    parserData->paramIndex = 0;
    parserData->lhsId = NULL;
    
    if (res = rule_returnValue( parserData )) exit( res );                                                  // <return value>

    return rule_otherReturnValue( parserData );                                                             // <other return value>

}

int rule_otherReturnValue ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type != T_COM) {
        cg_jump_to_end( parserData->insideFunc->id );
        return res;                                                                                         // e
    }

    if (res = get_next_token_and_apply_rule( parserData, "rule_returnValue" )) exit( res );                 // <return value>

    return rule_otherReturnValue( parserData );                                                             // <other return value>

}

int rule_returnValue ( Parser_data *parserData ) {
    // backup
    int res = 0;
    // number of return values is more than required
    if (parserData->paramIndex == parserData->insideFunc->outputTypes->length) exit( ERR_SEMANTIC_FUNC_PAR );
    if (res = rule_expression( parserData )) exit( res );

    Data_type type = get_param_or_retval_type( parserData->insideFunc, 1, parserData->paramIndex );
    
    if (parserData->expType != type) {
        if      (parserData->expType == T_NUM && type == T_INT) {
            if (!cg_convert_res_num2int( m_exp )) exit( ERR_INTERNAL );
        } 
        else if (parserData->expType == T_INT && type == T_NUM) {
            if (!cg_convert_res_int2num( m_exp )) exit( ERR_INTERNAL );
        } 
        else exit( ERR_SEMANTIC_FUNC_PAR );
    }
    
    if (!cg_function_retval_get_value( parserData->paramIndex )) exit( ERR_INTERNAL );

    parserData->paramIndex++;// backup

    return 0;

}

/*.......................................... WRITE ..........................................*/

int rule_writeArgumentList ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type == T_RBR) return res;

    parserData->paramIndex = 0;

    if (res = rule_expression( parserData )) return res;                                                    // <expression>
    
    if (!cg_frame_to_pass_param()) exit( ERR_INTERNAL );
    if (!cg_pass_param( parserData )) exit( ERR_INTERNAL );
    if (!cg_call( "write" )) exit( ERR_INTERNAL );

    return rule_otherWriteArgument( parserData );                                                           // <other write argument>

}

int rule_otherWriteArgument( Parser_data *parserData ) {

    int res = 0;
    
    if (parserData->token.type != T_COM) return res;

    if (res = get_next_token_and_apply_rule( parserData, "rule_expression" )) return res;                   // <expression>

    if (!cg_frame_to_pass_param()) exit( ERR_INTERNAL );
    if (!cg_pass_param( parserData )) exit( ERR_INTERNAL );
    if (!cg_call( "write" )) exit( ERR_INTERNAL );

    return rule_otherWriteArgument( parserData );                                                           // <other write argument>        

}
