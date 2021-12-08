/**
 * PROJECT: Implementation of imperative programming language compiler
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
        case ('l'):
            return T_NIL;
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
        case KW_NIL:
            return 'l';
        default:
            return ERR_SYNTAX;

    }
    
}

int get_next_token_and_check_type ( Parser_data *parserData, Data_type type ) {
    
    int tmp;
    if ((tmp = get_next_token( &parserData->token ))) return tmp;
    if (parserData->token.type != type) return ERR_SYNTAX;
    
    return 0;

}

int get_next_token_and_check_keyword ( Parser_data *parserData, Keyword keyword ) {

    int tmp;
    if ((tmp = get_next_token( &parserData->token ))) return tmp;
    if (parserData->token.type != T_KEY
        || parserData->token.attribute.keyword != keyword) return ERR_SYNTAX;
    
    return 0;

}

int get_next_token_and_apply_rule ( Parser_data *parserData, char *rulename ) {
    
    int tmp = 0;
    if ((tmp = get_next_token( &parserData->token ))) return tmp;

    if (!strcmp( rulename, "rule_expression" ))                 { if ((tmp = rule_expression( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_statementList"))          { if ((tmp = rule_statementList( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_optionalDefinition"))     { if ((tmp = rule_optionalDefinition( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_declarationParams" ))     { if ((tmp = rule_declarationParams( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_output" ))                { if ((tmp = rule_output( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_definitionParams" ))      { if ((tmp = rule_definitionParams( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_definitionParam" ))       { if ((tmp = rule_definitionParam( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_value"))                  { if ((tmp = rule_value( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_IDList" ))                { if ((tmp = rule_IDList( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_otherID" ))               { if ((tmp = rule_otherID( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_assignmentValues"))       { if ((tmp = rule_assignmentValues( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_otherAssignmentValue" ))  { if ((tmp = rule_otherAssignmentValue( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_functionCallParams" ))    { if ((tmp = rule_functionCallParams( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_callParam" ))             { if ((tmp = rule_callParam( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_assignmentValue" ))       { if ((tmp = rule_assignmentValue( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_returnValues" ))          { if ((tmp = rule_returnValues( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_returnValue" ))           { if ((tmp = rule_returnValue( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_writeArgumentList" ))     { if ((tmp = rule_writeArgumentList( parserData ))) return tmp; }
    else if (!strcmp( rulename, "rule_otherWriteArgument" ))    { if ((tmp = rule_otherWriteArgument( parserData ))) return tmp; }

    return 0;

}

bool parser_data_init ( Parser_data *parserData ) {

    int res = 0;

    // Parser data parameters

    parserData->lhsId = NULL;
    parserData->currentVar = NULL;
    parserData->currentFunc = NULL;
    parserData->insideFunc = NULL;
    parserData->expType = T_NDA;

    parserData->ifIndex = 0;
    parserData->inWhile = false;
    parserData->whileIndex = 0;
    parserData->currentDepth = 0;
    parserData->paramIndex = 0;

    // symbol table stack initialization
    sts_init( &parserData->STStack );
    parserData->globalSymTable = sts_push( &parserData->STStack, parserData->currentDepth );

    // identifier queue initialization
    q_init( &parserData->queue );

    // add built-in functions to global symbol table
    ADD_GLOBAL( "reads" );
    if ((res = st_add_param( parserData->currentFunc->outputTypes, KW_STRING ))) return false;
    parserData->currentFunc->ifdec = 1;
    parserData->currentFunc->ifdef = 1;

    ADD_GLOBAL( "readi" );
    if ((res = st_add_param( parserData->currentFunc->outputTypes, KW_INTEGER ))) return false;
    parserData->currentFunc->ifdec = 1;
    parserData->currentFunc->ifdef = 1;
    
    ADD_GLOBAL( "readn" );
    if ((res = st_add_param( parserData->currentFunc->outputTypes, KW_NUMBER ))) return false;
    parserData->currentFunc->ifdec = 1;
    parserData->currentFunc->ifdef = 1;

    ADD_GLOBAL( "write" );
    if ((res = st_add_param( parserData->currentFunc->outputTypes, ANY ))) return false;
    parserData->currentFunc->ifdec = 1;
    parserData->currentFunc->ifdef = 1;

    ADD_GLOBAL( "tointeger" );
    if ((res = st_add_param( parserData->currentFunc->outputTypes, KW_INTEGER ))) return false;
    if ((res = st_add_param( parserData->currentFunc->inputTypes, KW_NUMBER ))) return false;
    parserData->currentFunc->ifdec = 1;
    parserData->currentFunc->ifdef = 1;
    
    ADD_GLOBAL( "substr" );
    if ((res = st_add_param( parserData->currentFunc->inputTypes, KW_STRING ))) return false;
    if ((res = st_add_param( parserData->currentFunc->inputTypes, KW_INTEGER ))) return false;
    if ((res = st_add_param( parserData->currentFunc->inputTypes, KW_INTEGER ))) return false;
    if ((res = st_add_param( parserData->currentFunc->outputTypes, KW_STRING ))) return false;
    parserData->currentFunc->ifdec = 1;
    parserData->currentFunc->ifdef = 1;

    ADD_GLOBAL( "ord" );
    if ((res = st_add_param( parserData->currentFunc->inputTypes, KW_STRING ))) return false;
    if ((res = st_add_param( parserData->currentFunc->inputTypes, KW_INTEGER ))) return false;
    if ((res = st_add_param( parserData->currentFunc->outputTypes, KW_INTEGER ))) return false;
    parserData->currentFunc->ifdec = 1;
    parserData->currentFunc->ifdef = 1;

    ADD_GLOBAL( "chr" );
    if ((res = st_add_param( parserData->currentFunc->inputTypes, KW_INTEGER ))) return false;
    if ((res = st_add_param( parserData->currentFunc->outputTypes, KW_STRING ))) return false;
    parserData->currentFunc->ifdec = 1;
    parserData->currentFunc->ifdef = 1;
    
    return true;

}

int parse (  ) {
    
    int res = 0;
    // initialization of parser data structure
    Parser_data pData;
    Parser_data *parserData = &pData;
    if (!parser_data_init( parserData )) return ERR_INTERNAL; 

    // initialization of dynamic string for scanner
    Dynamic_string str1;
    Dynamic_string *scannerString = &str1;
    if (!ds_init( scannerString )) return ERR_INTERNAL;
    _token_string( scannerString );

    // initialization of dynamic string for code generator
    Dynamic_string str2;
    Dynamic_string *codeString = &str2;
    if (!ds_init( codeString )) return ERR_INTERNAL;
    _code_string( codeString );

    // <prologue>
    if ((res = rule_prologue( parserData ))) return res;
    // <function list>
    if ((res = rule_functionList( parserData ))) return res;
    if ((res = search_undefined( *parserData->globalSymTable ))) return res;
    // deallocation of memory for auxiliary structures
    q_dispose( &parserData->queue );
    sts_dispose( parserData->STStack.top );

    // print result program
    cg_output( stdout );

    ds_free( scannerString );

    return res; 

}

/*........................................ RECURSIVE TOP-DOWN PARSER ........................................ */

int rule_prologue ( Parser_data *parserData ) {
    
    int tmp;
    if ((tmp = get_next_token_and_check_keyword ( parserData, KW_REQUIRE ))) return tmp;                    // REQUIRE
    if ((tmp = get_next_token( &parserData->token ))) return tmp;                                           // "ifj21"
    if (strcmp( parserData->token.attribute.string->str, "ifj21" )) return ERR_SEMANTIC_OTHER;
    // prologue OK -> generate start of program 
    if (!cg_start(  )) return ERR_INTERNAL;
    if ((tmp = get_next_token( &parserData->token ))) return tmp;
    return 0;

}

/*........................................ FUNCTION HEADERS ........................................ */

int rule_functionList ( Parser_data *parserData ) {

    int res = 0;
    
    parserData->lhsId = NULL;
    parserData->currentVar = NULL;
    parserData->currentFunc = NULL;
    parserData->insideFunc = NULL;

    if (parserData->token.type == T_EOF) {

        return 0;

    } 

    else if (parserData->token.attribute.keyword == KW_GLOBAL) {
            
        if ((res = rule_functionDeclaration( parserData ))) return res;                                 // <function declaration>
        return rule_functionList( parserData );                                                         // <function list>

    }

    else if (parserData->token.attribute.keyword == KW_FUNCTION) {
        // reset label indexing
        parserData->ifIndex = 0;    
        parserData->whileIndex = 0;

        if ((res = rule_functionDefinition( parserData ))) return res;                                  // <function definition>
        return rule_functionList( parserData );                                                         // <function list>

    }

    else if (parserData->token.type == T_IDE) {
        
        if ((res = rule_entryPoint( parserData ))) return res;                                          // <entry point>
        return rule_functionList( parserData );                                                         // <function list>

    }

    else return ERR_SYNTAX;

}

int rule_entryPoint ( Parser_data *parserData ) {

    int res = 0;                                                                                            // ID
    
    SEARCH_GLOBAL( parserData->token.attribute.string->str );
    if (res == 0) return ERR_SEMANTIC_UNDEF_VAR;
    res = 0;

    if (!cg_frame_to_pass_param (  )) return ERR_INTERNAL;
    if ((res = get_next_token_and_check_type( parserData, T_LBR ))) return res;                             // (
    if ((res = get_next_token_and_apply_rule( parserData, "rule_functionCallParams" ))) return res;         // <function call params>
    if (parserData->token.type != T_RBR) return ERR_SYNTAX;                                                 // )

    if (!cg_call( parserData->currentFunc->id )) return ERR_INTERNAL;

    if ((res = get_next_token( &parserData->token ))) return res;
    return 0;

}

int rule_functionDeclaration ( Parser_data *parserData ) {

    int res = 0;                                                                                            // GLOBAL

    if ((res = get_next_token_and_check_type( parserData, T_IDE ))) return res;                             // ID
    SEARCH_GLOBAL( parserData->token.attribute.string->str );
    if (res == 1) return ERR_SEMANTIC_UNDEF_VAR; // attempt of function redeclaration or declaration after definition
    ADD_GLOBAL( parserData->token.attribute.string->str );

    if ((res = get_next_token_and_check_type( parserData, T_COL ))) return res;                             // :
    if ((res = get_next_token_and_check_keyword( parserData, KW_FUNCTION ))) return res;                    // FUNCTION
    if ((res = get_next_token_and_check_type( parserData, T_LBR ))) return res;                             // (

    if ((res = get_next_token_and_apply_rule( parserData, "rule_declarationParams" ))) return res;          // <declaration params>
    if (parserData->token.type != T_RBR) return ERR_SYNTAX;                                                 // )

    if ((res = get_next_token_and_apply_rule( parserData, "rule_output" ))) return res;                     // <output>
    
    parserData->currentFunc->ifdec = 1;
    return 0;

}

int rule_declarationParams ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type == T_RBR) return res;
    if ((res = st_add_param( parserData->currentFunc->inputTypes, parserData->token.attribute.keyword ))) return res;

    return rule_otherDeclarationParam( parserData );

}

int rule_otherDeclarationParam ( Parser_data *parserData ) {

    int res = 0;
    
    if ((res = get_next_token( &parserData->token ))) return res;

    if (parserData->token.type == T_RBR) return res;

    if ((res = get_next_token( &parserData->token ))) return res;
    if ((res = st_add_param( parserData->currentFunc->inputTypes, parserData->token.attribute.keyword ))) return res;

    return rule_otherDeclarationParam( parserData );

}

int rule_functionDefinition ( Parser_data *parserData ) {

    int res = 0;                                                                                            // FUNCTION
    
    if ((res = get_next_token_and_check_type( parserData, T_IDE ))) return res;                             // ID

    SEARCH_GLOBAL( parserData->token.attribute.string->str );
    if (res == 0) {
        ADD_GLOBAL( parserData->token.attribute.string->str );
        parserData->currentFunc->ifdef = 1;
    }
    else { // attempt of function redefinition?
        if (parserData->currentFunc->ifdef == 1) return ERR_SEMANTIC_UNDEF_VAR;
        else parserData->currentFunc->ifdef = 1;
        res = 0;
    }
    
    parserData->insideFunc = parserData->currentFunc;
    if (!cg_function_header( parserData->insideFunc->id )) return ERR_INTERNAL;

    if ((res = get_next_token_and_check_type( parserData, T_LBR ))) return res;                             // (
    
    parserData->currentDepth++;
    sts_push( &parserData->STStack, parserData->currentDepth );

    if ((res = get_next_token_and_apply_rule( parserData, "rule_definitionParams" ))) return res;           // <definition params>
    if (parserData->token.type != T_RBR) return ERR_SYNTAX;                                                 // )
    if ((res = get_next_token_and_apply_rule( parserData, "rule_output" ))) return res;                     // <output>
    if ((res = rule_statementList( parserData ))) return res;                                               // <statement list>
    
    sts_pop( &parserData->STStack );
    parserData->currentDepth--;
    
    if (parserData->token.attribute.keyword != KW_END) return ERR_SYNTAX;                                   // END
    if (!cg_function_return( parserData->insideFunc->id )) return ERR_INTERNAL; 

    if ((res = get_next_token( &parserData->token ))) return res; 
    return 0;
    
}

int rule_definitionParams ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type == T_RBR) {
        if (parserData->currentFunc->inputTypes->length == 0) return res;
        else return ERR_SEMANTIC_FUNC_PAR;
    }
    parserData->paramIndex = 0;

    if ((res = rule_definitionParam( parserData ))) return res; 

    return rule_otherDefinitionParam( parserData );

}

int rule_otherDefinitionParam ( Parser_data *parserData ) {

    int res = 0;

    if ((res = get_next_token( &parserData->token ))) return res;                                           // ,
    
    if (parserData->token.type != T_COM) {
        if (parserData->currentFunc->ifdec) {
            if (parserData->currentFunc->inputTypes->length == parserData->paramIndex) return res;
            else return ERR_SEMANTIC_FUNC_PAR;
        } else return res;
    }

    if ((res = get_next_token_and_apply_rule( parserData, "rule_definitionParam" ))) return res;            // <definition param>
    
    return rule_otherDefinitionParam( parserData );                                                         // <other definition param>

}

int rule_definitionParam ( Parser_data *parserData ) {

    int res = 0;
    
    ADD_LOCAL( parserData->token.attribute.string->str );                                                   // ID
    
    if ((res = get_next_token_and_check_type( parserData, T_COL ))) return res;                             // :
    if ((res = get_next_token( &parserData->token ))) return res;                                           // <type>
    // to control if types are the same both in declared and in defined versions
    if (parserData->currentFunc->ifdec == 1) {

        if (parserData->currentFunc->inputTypes->length == parserData->paramIndex) return ERR_SEMANTIC_FUNC_PAR;
        if (parserData->currentFunc->inputTypes->str[parserData->paramIndex] != convert_data_type_to_char( parserData->token.attribute.keyword )) {
            return ERR_SEMANTIC_FUNC_PAR;
        }
        
    } 
    // to add types to function
    else {
        
        if ((res = st_add_param( parserData->currentFunc->inputTypes, parserData->token.attribute.keyword ))) return res; 

    }
    
    // assign type to variable
    if ((res = st_add_type( &parserData->token, parserData->currentVar ))) return res; 

    if (parserData->currentFunc->ifdef) {
        if (!cg_function_param( parserData )) return ERR_INTERNAL;
    }

    parserData->paramIndex++;

    return 0;

}

int rule_output ( Parser_data *parserData ) {
    
    int res = 0;

    if (parserData->token.type != T_COL) {
        if (parserData->currentFunc->ifdec) {
            if (parserData->currentFunc->outputTypes->length == 0) return res;
            else return ERR_SEMANTIC_FUNC_PAR;
        } else return res;
    }

    parserData->paramIndex = 0;
    if ((res = get_next_token( &parserData->token ))) return res;                                             // <type>
    if (parserData->currentFunc->ifdec) {
        
        if (parserData->currentFunc->outputTypes->str[parserData->paramIndex] != convert_data_type_to_char( parserData->token.attribute.keyword )) return ERR_SEMANTIC_FUNC_PAR;

    }
    else {

        if ((res = st_add_param( parserData->currentFunc->outputTypes, parserData->token.attribute.keyword ))) return res; 

    }
    
    if (!cg_function_retval( parserData->paramIndex )) return ERR_INTERNAL;
    parserData->paramIndex++;

    if ((res = get_next_token( &parserData->token ))) return res; 
    return rule_otherOutputType( parserData );

}

int rule_otherOutputType ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type != T_COM) {
        if (parserData->currentFunc->ifdec) {
            if (parserData->paramIndex == parserData->currentFunc->outputTypes->length) return res;
            else return ERR_SEMANTIC_FUNC_PAR;
        } else return res;
    }

    if ((res = get_next_token( &parserData->token ))) return res;                                           // <type>
    if (parserData->currentFunc->ifdec) {
        
        if (parserData->currentFunc->outputTypes->str[parserData->paramIndex] != convert_data_type_to_char( parserData->token.attribute.keyword )) return ERR_SEMANTIC_FUNC_PAR;
        
    }
    else {

        if ((res = st_add_param( parserData->currentFunc->outputTypes, parserData->token.attribute.keyword ))) return res; 

    }

    if (!cg_function_retval( parserData->paramIndex )) return ERR_INTERNAL;
    parserData->paramIndex++;

    if ((res = get_next_token( &parserData->token ))) return res; 
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
        // $1.0
        // <statement> -> <ID list> <assignment values>
        if (res == 0) {

            if ((res = rule_IDList( parserData ))) return res;                                              // <ID list>
            if ((res = rule_assignmentValues( parserData ))) return res;                                    // <assignment values>
            if (parserData->queue.top) return ERR_SEMANTIC_OTHER;  // if queue is not empty -> semantic error

        } 
        // $1.5
        // <statement> -> ID (<function call param list>) 
        else if (res == 1) {
            
            res = 0;

            if (!cg_frame_to_pass_param(  )) return ERR_INTERNAL; 
            if ((res = get_next_token_and_check_type( parserData, T_LBR ))) return res;                     // (
            if ((res = get_next_token_and_apply_rule( parserData, "rule_functionCallParams" ))) return res; // <function call param list>
            if (parserData->token.type != T_RBR) return ERR_SYNTAX;                                         // )
            if (!cg_call( parserData->currentFunc->id )) return ERR_INTERNAL;       

        }
        if (parserData->token.type == T_RBR) {
            if ((res = get_next_token( &parserData->token ))) return res; 
        }

        parserData->lhsId = NULL;
        parserData->currentFunc = NULL;
        return rule_statementList( parserData );

    }

    // $2
    // <statement> -> LOCAL ID : <type> <optional definition>
    else if (parserData->token.attribute.keyword == KW_LOCAL) {                                             // LOCAL

        if ((res = get_next_token_and_check_type( parserData, T_IDE ))) return res;                         // ID
        
        

        SEARCH_GLOBAL( parserData->token.attribute.string->str ); // variable with function name -> error
        if (res == 1) return ERR_SEMANTIC_UNDEF_VAR; 
        SEARCH_LOCAL( parserData->token.attribute.string->str ); // attempt of variable redefinition on the same lvl
        if (res == 1) return ERR_SEMANTIC_UNDEF_VAR; 

        ADD_LOCAL( parserData->token.attribute.string->str );
        parserData->lhsId = parserData->currentVar;
        
        if (parserData->inWhile) {
            if (!cg_while_start_jump_over_declaration( parserData )) return ERR_INTERNAL;
        }

        if ((res = get_next_token_and_check_type( parserData, T_COL ))) return res;                         // :
        if ((res = get_next_token( &parserData->token ))) return res;                                       // <type>
        if ((res = st_add_type( &parserData->token, parserData->lhsId ))) return res;

        if (!cg_declare_var( parserData )) return ERR_INTERNAL;
           
        if ((res = get_next_token_and_apply_rule( parserData, "rule_optionalDefinition" ))) return res;     // <optional definition>

        if (parserData->inWhile) {
            if (!cg_while_end_jump_over_declaration( parserData )) return ERR_INTERNAL;
        }

        parserData->lhsId = NULL;
        parserData->currentFunc = NULL;
        return rule_statementList( parserData );

    }

    // $3
    // <statement> -> IF <expression> THEN <statement list> ELSE <statement list> END <statement list>
    else if (parserData->token.attribute.keyword == KW_IF) {                                                // IF

        int currentIndex = parserData->ifIndex;

        if ((res = get_next_token_and_apply_rule( parserData, "rule_expression" ))) return res;             // <expression>
        if (parserData->token.attribute.keyword != KW_THEN) return ERR_SYNTAX;                              // THEN

        parserData->ifIndex++;
        parserData->currentDepth++;
        sts_push( &parserData->STStack, parserData->currentDepth );

        if (!cg_if_header( currentIndex, parserData->insideFunc->id )) return ERR_INTERNAL; 

        if ((res = get_next_token_and_apply_rule( parserData, "rule_statementList" ))) return res;          // <statement list>
        if (parserData->token.attribute.keyword != KW_ELSE) return ERR_SYNTAX;                              // ELSE
        
        if (!cg_if_else( currentIndex, parserData->insideFunc->id )) return ERR_INTERNAL; 

        st_dispose( &parserData->STStack.top->symTable );
        if ((res = get_next_token_and_apply_rule( parserData, "rule_statementList" ))) return res;          // <statement list>
        
        sts_pop( &parserData->STStack );
        parserData->currentDepth--;
        
        if (parserData->token.attribute.keyword != KW_END) return ERR_SYNTAX;                               // END

        if (!cg_if_end( currentIndex, parserData->insideFunc->id )) return ERR_INTERNAL; 
        
        if ((res = get_next_token( &parserData->token ))) return res;
        return rule_statementList( parserData );

    }

    // $4
    // <statement> -> WHILE <expression> DO <statement list> END <statement list>
    else if (parserData->token.attribute.keyword == KW_WHILE) {                                             // WHILE

        int currentIndex = parserData->whileIndex;

        if (!cg_while_header( currentIndex, parserData->insideFunc->id )) return ERR_INTERNAL; 
        if ((res = get_next_token_and_apply_rule( parserData, "rule_expression" ))) return res;             // <expression>
        if (parserData->token.attribute.keyword != KW_DO) return ERR_SYNTAX;                                // DO
        
        if (!cg_while_condition( currentIndex, parserData->insideFunc->id )) return ERR_INTERNAL; 
        
        parserData->inWhile = true;
        parserData->whileIndex++;
        parserData->currentDepth++;
        sts_push( &parserData->STStack, parserData->currentDepth );

        if ((res = get_next_token_and_apply_rule( parserData, "rule_statementList" ))) return res;          // <statement list>
        if (parserData->token.attribute.keyword != KW_END) return ERR_SYNTAX;                               // END
        
        if (!cg_while_end( currentIndex, parserData->insideFunc->id )) return ERR_INTERNAL;

        sts_pop( &parserData->STStack );
        parserData->currentDepth--;
        parserData->inWhile = false;

        if ((res = get_next_token( &parserData->token ))) return res;
        return rule_statementList( parserData );                                                            // <statement list>
        
    }

    // $5
    // <statement> -> RETURN <return list>
    else if (parserData->token.attribute.keyword == KW_RETURN) {                                            // RETURN
                                                                                                    
        if ((res = get_next_token_and_apply_rule( parserData, "rule_returnValues" ))) return res;           // <return list>
        
        return rule_statementList( parserData );                                                            // <statement list>

    }

    // $6
    // <statement> -> WRITE (<write argument list>)
    else if (parserData->token.attribute.keyword == KW_WRITE) {                                             // WRITE

        SEARCH_GLOBAL( "write" );
        
        if ((res = get_next_token_and_check_type( parserData, T_LBR ))) return res;                         // (
        if ((res = get_next_token_and_apply_rule( parserData, "rule_writeArgumentList" ))) return res;      // <write argument list>
        if (parserData->token.type != T_RBR) return ERR_SYNTAX;                                             // )
        
        if ((res = get_next_token( &parserData->token ))) return res; 
        return rule_statementList( parserData );
        
    }

    // $
    // <statement> -> ε
    else {
        return 0;
    }

}

/*.......................................... IDENTIFIER LIST ..........................................*/

int rule_IDList ( Parser_data *parserData ) {

    int res = 0;                                                                                            // ID
    
    SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
    if (res == 0) return ERR_SEMANTIC_UNDEF_VAR;
    res = 0;
    q_push( &parserData->queue, parserData->currentVar );

    if ((res = get_next_token( &parserData->token ))) return res; 

    return rule_otherID( parserData );                                                                      // <other ID>

}

int rule_otherID ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type != T_COM) return res;                                                        // ,
    if ((res = get_next_token_and_check_type( parserData, T_IDE ))) return res;                             // ID
    SEARCH_GLOBAL( parserData->token.attribute.string->str );
    if (res == 1) return ERR_SEMANTIC_UNDEF_VAR;
    SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
    if (res == 0) return ERR_SEMANTIC_UNDEF_VAR;
    res = 0;
    q_push( &parserData->queue, parserData->currentVar );
   
    if ((res = get_next_token( &parserData->token ))) return res; 
    return rule_otherID( parserData );                                                                      // <other ID>

}

/*.......................................... ASSIGNMENT VALUES ..........................................*/

int rule_assignmentValues( Parser_data *parserData ) {

    int res = 0;
    
    if (parserData->token.type != T_ASS) return ERR_SYNTAX;                                                 // =
    if ((res = get_next_token_and_apply_rule( parserData, "rule_assignmentValue" ))) return res;            // <assignment value>
    return rule_otherAssignmentValue( parserData );                                                         // <other assignment value>

}

int rule_otherAssignmentValue( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type != T_COM) return res;                                                        // ,
    if ((res = get_next_token_and_apply_rule( parserData, "rule_assignmentValue"))) return res;             // <assignment value>
    return rule_otherAssignmentValue( parserData );                                                         // <other assignment value>

}

int rule_assignmentValue ( Parser_data *parserData ) {
    
    int res = 0;
    // built-in function
    if (IS_BUILTIN(parserData->token)) {

        SEARCH_BUILTIN( parserData->token );
        res = 0;
        if (!cg_frame_to_pass_param(  )) return ERR_INTERNAL; 
        if ((res = get_next_token_and_check_type( parserData, T_LBR ))) return res;                         // (
        if ((res = get_next_token_and_apply_rule( parserData, "rule_functionCallParams" ))) return res;     // <function call params>
        if (parserData->token.type != T_RBR) return ERR_SYNTAX;                                             // )
        if (!cg_call( parserData->currentFunc->id )) return ERR_INTERNAL; 

        parserData->currentVar = q_pop( &parserData->queue );                                               // value return
        Data_type retvalType = get_param_or_retval_type( parserData->currentFunc, 1, 0 );
        if (parserData->currentVar->type != retvalType ) {
            if (retvalType == T_INT && parserData->currentVar->type == T_NUM) {
                if (!cg_convert_res_int2num( m_ret )) return ERR_INTERNAL;
            }
            else return ERR_SEMANTIC_FUNC_PAR;
        }
        if (!cg_get_retval( parserData->currentVar, 0 )) return ERR_INTERNAL; 

    }
    else if (parserData->token.type == T_IDE) {

        SEARCH_GLOBAL( parserData->token.attribute.string->str );
        // function
        if (res == 1) {
            res = 0;
            if (!cg_frame_to_pass_param(  )) return ERR_INTERNAL; 
            if ((res = get_next_token_and_check_type( parserData, T_LBR ))) return res;                         // (
            if ((res = get_next_token_and_apply_rule( parserData, "rule_functionCallParams" ))) return res;     // <function call params>
            if (parserData->token.type != T_RBR) return ERR_SYNTAX;                                             // )
            if (!cg_call( parserData->currentFunc->id )) return ERR_INTERNAL;                  

            for (int i = 0; i < parserData->currentFunc->outputTypes->length; i++) {                            // value return
                parserData->currentVar = q_pop( &parserData->queue );
                Data_type retvalType = get_param_or_retval_type( parserData->currentFunc, 1, i );
                if (parserData->currentVar->type != retvalType ) {
                    if (retvalType == T_INT && parserData->currentVar->type == T_NUM) {
                        if (!cg_convert_res_int2num( m_ret )) return ERR_INTERNAL;
                    }
                    else return ERR_SEMANTIC_FUNC_PAR;
                }
                if (!cg_get_retval( parserData->currentVar, i )) return ERR_INTERNAL; 
            }

        } 
        // variable
        else {

            parserData->lhsId = q_pop( &parserData->queue );
            if ((res = rule_expression( parserData ))) return res; 

        }
        
    }
    // constant
    else {

        parserData->lhsId = q_pop( &parserData->queue );
        if ((res = rule_expression( parserData ))) return res; 

    }

    return 0;

}

/*.......................................... FUNCTION CALL ..........................................*/

int rule_functionCallParams( Parser_data *parserData ) {

    int res = 0;

    parserData->currentFunc->ifuse = 1;
    if (parserData->token.type == T_RBR) {
        if (parserData->currentFunc->inputTypes->length == 0) return res;
        else return ERR_SEMANTIC_FUNC_PAR; 
    }

    parserData->paramIndex = 0;
    if ((res = rule_callParam( parserData ))) return res;                                                   // <call param>  
    if ((res = get_next_token( &parserData->token ))) return res; 

    return rule_otherCallParam( parserData );                                                               // <other call param>
    
}

int rule_otherCallParam ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type != T_COM) {                                                                  // ,
        if (parserData->currentFunc->inputTypes->length == parserData->paramIndex)
        return res;
        else return ERR_SEMANTIC_FUNC_PAR;
    }

    if ((res = get_next_token_and_apply_rule( parserData, "rule_callParam" ))) return res;                  // <call param>
    if ((res = get_next_token( &parserData->token ))) return res; 
    
    return rule_otherCallParam( parserData );                                                               // <other call param>

}

int rule_callParam ( Parser_data *parserData ) {
    
    int res = 0;

    if (parserData->currentFunc->inputTypes->length == parserData->paramIndex) return ERR_SEMANTIC_FUNC_PAR; 

    if (parserData->token.type == T_IDE) {
        // parserData->currentVar <- search
        SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
        if (res == 0) return ERR_SEMANTIC_UNDEF_VAR; 
        res = 0;

        if (parserData->currentVar->type != get_param_or_retval_type( parserData->currentFunc, 0, parserData->paramIndex )) return ERR_SEMANTIC_FUNC_PAR; 

        if (!cg_pass_param_light( parserData )) return ERR_INTERNAL; 
        
    }
    
    else if (IS_VAL( parserData->token )) {

        if (parserData->token.type != get_param_or_retval_type( parserData->currentFunc, 0, parserData->paramIndex )) return ERR_SEMANTIC_FUNC_PAR;
        if (!cg_pass_param_light( parserData )) return ERR_INTERNAL; 

    }

    else return ERR_SYNTAX; 

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

        if ((res = get_next_token_and_apply_rule( parserData, "rule_value" ))) return res;
        
    }

    return res;

}

int rule_value ( Parser_data *parserData ) {
    
    int res = 0;
    
    if ((parserData->token.type == T_IDE || parserData->token.type == T_KEY) || IS_VAL( parserData->token ) 
    || parserData->token.type == T_LBR || parserData->token.type == T_LEN) {
        
        if (IS_VAL( parserData->token ) || parserData->token.type == T_LBR || parserData->token.type == T_LEN) {

            if ((res = rule_expression( parserData ))) return res; 
            
            return res;    

        }
        // <value> -> ID
        if (parserData->token.type != T_KEY) SEARCH_ALL_LOCAL( parserData->token.attribute.string->str );
        if (res == 1) {      // not func
        
            res = 0;

            if ((res = rule_expression( parserData ))) return res;

            return res;
            
        } else if (IS_BUILTIN( parserData->token )) {
            
            SEARCH_BUILTIN(parserData->token);  
            
        } // if built-in

        else { // if not

            SEARCH_GLOBAL( parserData->token.attribute.string->str );

        }

        if (parserData->currentFunc) {

            res = 0;

            if (!cg_frame_to_pass_param(  )) return ERR_INTERNAL; 
            if ((res = get_next_token_and_check_type( parserData, T_LBR ))) return res;                         // (
            if ((res = get_next_token_and_apply_rule( parserData, "rule_functionCallParams" ))) return res;     // <function call params>
            if (parserData->token.type != T_RBR) return ERR_SYNTAX;                                             // )
            if (!cg_call( parserData->currentFunc->id )) return ERR_INTERNAL; 
            Data_type type = get_param_or_retval_type( parserData->currentFunc, 1, 0 );
            if (parserData->lhsId->type != type) {
                if      (parserData->lhsId->type == T_NUM && type == T_INT) {
                    if (!cg_convert_res_int2num( m_ret )) return ERR_INTERNAL; 
                } 
                else return ERR_SEMANTIC_FUNC_PAR;
            }
            if (!cg_get_retval( parserData->lhsId, 0 )) return ERR_INTERNAL; 

            if ((res = get_next_token( &parserData->token ))) return res; 
            return 0;

        } else return ERR_SEMANTIC_UNDEF_VAR;

    } else return ERR_SYNTAX;
    
}

/*.......................................... RETURN ..........................................*/

int rule_returnValues ( Parser_data *parserData ) {
    
    int res = 0;
    
    if ((!IS_VAL( parserData->token )) && parserData->token.type != T_IDE
    && parserData->token.type != T_LEN && parserData->token.type != T_LBR) {                                // e
        if (!cg_jump_to_end( parserData->insideFunc->id )) return ERR_INTERNAL; 
        return res;
    } 
    parserData->paramIndex = 0;
    parserData->lhsId = NULL;
    
    if ((res = rule_returnValue( parserData ))) return res;                                                 // <return value>

    return rule_otherReturnValue( parserData );                                                             // <other return value>

}

int rule_otherReturnValue ( Parser_data *parserData ) {

    int res = 0;

    if (parserData->token.type != T_COM) {
        if (!cg_jump_to_end( parserData->insideFunc->id )) return ERR_INTERNAL;
        return res;                                                                                         // e
    }

    if ((res = get_next_token_and_apply_rule( parserData, "rule_returnValue" ))) return res;                // <return value>

    return rule_otherReturnValue( parserData );                                                             // <other return value>

}

int rule_returnValue ( Parser_data *parserData ) {
    
    int res = 0;
    // number of return values is more than required
    if (parserData->paramIndex == parserData->insideFunc->outputTypes->length) return ERR_SEMANTIC_FUNC_PAR;
    if ((res = rule_expression( parserData ))) return res;                                                  // <expression>
    
    Data_type type = get_param_or_retval_type( parserData->insideFunc, 1, parserData->paramIndex );
    
    if (parserData->expType != type && parserData->expType != T_NIL) {
        if (parserData->expType == T_INT && type == T_NUM) {
            if (!cg_convert_res_int2num( m_exp )) return ERR_INTERNAL;
        } 
        else return ERR_SEMANTIC_FUNC_PAR;
    }
    
    if (!cg_function_retval_get_value( parserData->paramIndex )) return ERR_INTERNAL; 

    parserData->paramIndex++;

    return 0;

}

/*.......................................... WRITE ..........................................*/

int rule_writeArgumentList ( Parser_data *parserData ) {

    int res = 0;
    if (parserData->token.type == T_RBR) return res;

    parserData->lhsId = NULL;
    parserData->paramIndex = 0;
    if ((res = rule_expression( parserData ))) return res;                                                  // <expression>
    
    if (!cg_frame_to_pass_param(  )) return ERR_INTERNAL; 
    if (!cg_pass_param( parserData )) return ERR_INTERNAL; 
    if (!cg_call( "write" )) return ERR_INTERNAL; 

    return rule_otherWriteArgument( parserData );                                                           // <other write argument>

}

int rule_otherWriteArgument ( Parser_data *parserData ) {

    int res = 0;
    
    if (parserData->token.type != T_COM) return res;

    if ((res = get_next_token_and_apply_rule( parserData, "rule_expression" ))) return res;                 // <expression>

    if (!cg_frame_to_pass_param(  )) return ERR_INTERNAL; 
    if (!cg_pass_param( parserData )) return ERR_INTERNAL; 
    if (!cg_call( "write" )) return ERR_INTERNAL; 

    return rule_otherWriteArgument( parserData );                                                           // <other write argument>        

}
