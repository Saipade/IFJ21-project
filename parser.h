/*

*/

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "scanner.h"
#include "symtable.h"

#define IS_VAL(TOKEN)                                                                                               \
    TOKEN.type == T_INT ||                                                                                          \
    TOKEN.type == T_NUM ||                                                                                          \
    TOKEN.type == T_STR ||                                                                                          \
    TOKEN.type == T_NIL

#define IS_DATA_TYPE(TOKEN)                                                                                         \
    TOKEN.type == T_KEY                                                                                             \
    && (TOKEN.attribute.keyword == KW_INTEGER                                                                       \
    || TOKEN.attribute.keyword == KW_NUMBER                                                                         \
    || TOKEN.attribute.keyword == KW_STRING                                                                         \
    || TOKEN.attribute.keyword == KW_NIL)                                                                           

#define IS_BUILTIN(TOKEN)                                                                                           \
    TOKEN.type == T_KEY                                                                                             \
    && (TOKEN.attribute.keyword == KW_READI                                                                         \
    || TOKEN.attribute.keyword == KW_READN                                                                          \
    || TOKEN.attribute.keyword == KW_READS                                                                          \
    || TOKEN.attribute.keyword == KW_WRITE                                                                          \
    || TOKEN.attribute.keyword == KW_TOINTEGER                                                                      \
    || TOKEN.attribute.keyword == KW_SUBSTR                                                                         \
    || TOKEN.attribute.keyword == KW_ORD                                                                            \
    || TOKEN.attribute.keyword == KW_CHR)                                                                           \

// id queue element
typedef struct QElem {

    struct QElem *next;
    Item_data *data;

} Queue_item;

// id queue structure for multiple assignment
typedef struct {

    Queue_item *top;
    Queue_item *end;

} Queue;

typedef struct {

    Token token;                // token got by get_next_token func
    Queue queue;                // queue structure for multiple assignment
    Sym_table symTable[20];     // 
    SymTable_Stack_item SStack; // symbol table stack 
    Item_data *lhsId;           // left-hand  side func/var identifier
    Item_data *rhsId;           // right-hand side func/var identifier
    Item_data *currentVar;      // pointer to current variable
    Item_data *currentFunc;     // pointer to current function
    Item_data *insideFunc;      // pointer on function that parser currently in

    bool inLoop;                // if parser is in if-statement/while cycle

    int currentDepth;           // depth for local symtables
    int whereAmI;               // function declaration or function definition
    int paramIndex;             // index of function parameter/retval

} Parser_data;


/**
 * @brief Pushes item to queue
 * @param queue identifier queue
 * @param item item to be pushed
 * @return the void is cold
 */
void q_push ( Queue *queue, Item_data *item );

/**
 * @brief Pops item from queue
 * @param queue identifier queue
 */
Item_data *q_pop ( Queue *queue );

/**
 * @brief Initializes the queue
 * @param queue identifier queue
 */
void q_init ( Queue *queue );

/**
 * @brief Destroy the queue
 * @param queue identifier queue
 */
void q_dispose ( Queue *queue );

/**
 * @brief Auxiliary function, returns function param/retval type on required index
 * @param function given function
 * @param mode 0 - param, 1 - retval
 * @param pos value position
 * @return data type
 */
Data_type get_param_or_retval_type ( Item_data *function, bool mode, int pos );

/**
 * @brief Auxiliary function, type keyword to character
 * @param dataType given keyword
 * @return Character representing type
 */
char convert_data_type_to_char ( Keyword dataType );

/**
 * @brief Initializes data structure required for syntax analysis and semantic analysis
 * @param parserData pointer to structure
 * @return Error
 */
bool parser_data_init ( Parser_data *parserData );

/**
 * @brief Main function for code syntax analysis. General rule: <program> -> <prologue> <function list>
 * @return Error code
 */
int parse (  );

/**
 * @brief <prologue> -> require "ifj21"
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_prologue ( Parser_data *parserData );

/**
 * @brief <function list> -> <function declaration> <function list> | <function declaration> <function list>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_functionList ( Parser_data *parserData );

/**
 * @brief <function declaration> -> GLOBAL ID : FUNCTION ( <declaration params> ) <output>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_functionDeclaration ( Parser_data *parserData );

/**
 * @brief <declaration params> -> ε | TYPE <other declaration param>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_declarationParams ( Parser_data *parserData );

/**
 * @brief <declaration params> -> ε | TYPE <other declaration param>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_otherDeclarationParam ( Parser_data *parserData );

/**
 * @brief <function defenition> -> FUNCTION ID ( <definition params> ) <output> <statement list> END
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_functionDefinition ( Parser_data *parserData );

/**
 * @brief <definition params> -> ε | <definition param> <other definition param>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_definitionParams ( Parser_data *parserData );

/**
 * @brief <other definition param> -> ε | , <definition param> <other definition param>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_otherDefinitionParam ( Parser_data *parserData );

/**
 * @brief <definition param> -> ID : TYPE
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_definitionParam ( Parser_data *parserData );

/**
 * @brief <output> -> ε | : TYPE <other output type>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_output ( Parser_data *parserData );

/**
 * @brief <other output type> -> ε | , TYPE <other output type>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_otherOutputType ( Parser_data *parserData );

/**
 * @brief <statement list> -> ε | <statement> <statement list> | any legal function body statement
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_statementList ( Parser_data *parserData );

/**
 * @brief <ID list> -> ID <other ID>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_IDList( Parser_data *parserData );

/**
 * @brief <other ID> -> ε | , ID <other ID>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_otherID( Parser_data *parserData );
/**
 * @brief <assignment values> -> = <assignment value> <other assignment value>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_assignmentValues( Parser_data *parserData );

/**
 * @brief <other assignment value> -> ε | , <assignment value> <other assignment value>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_otherAssignmentValue( Parser_data *parserData );

/**
 * @brief <assignment value> -> ID (<function call param list>) | <expression>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_assignmentValue( Parser_data *parserData );

/**
 * @brief <function call params> -> <call param> <other call param>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_functionCallParams( Parser_data *parserData );


/**
 * @brief <other call param> -> ε | , <call param> <other call param>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_otherCallParam ( Parser_data *parserData );

/**
 * @brief Processes param before function call
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_callParam ( Parser_data *parserData );

/**
 * @brief <optional definition> -> ε | = <value>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_optionalDefinition ( Parser_data *parserData );

/**
 * @brief <value> -> ID (<argument list>) | <value> -> <expression>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_value ( Parser_data *parserData );

/**
 * @brief <write argument list> -> ε | <expression> <other write argument>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_writeArgumentList( Parser_data *parserData );

/**
 * @brief <other write argument> -> ε | , <expression> <other write argument>
 * @param parserData syntax analysis data structure
 * @return Error code
 */
int rule_otherWriteArgument( Parser_data *parserData );

#endif