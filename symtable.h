#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "string_processor.h"
#include "scanner.h"

// found -> res = 1; not found -> res = 0
#define SEARCH_GLOBAL(STR)                                                                                                      \
    do {                                                                                                                        \
        Item_data *tmp = parserData->currentFunc;                                                                               \
        parserData->currentFunc = st_search( *parserData->globalSymTable, STR );                                                 \
        if (parserData->currentFunc == NULL) {                                                                                  \
            res = 0;                                                                                                            \
            parserData->currentFunc = tmp;                                                                                      \
        }                                                                                                                       \
        else res = 1;                                                                                                           \
    } while (0)

#define SEARCH_LOCAL(STR)                                                                                                       \
    do {                                                                                                                        \
        Item_data *tmp = parserData->currentVar;                                                                                \
        parserData->currentVar = st_search( parserData->STStack.top->symTable, STR );                                           \
        if (parserData->currentVar == NULL) {                                                                                   \
            res = 0;                                                                                                            \
            parserData->currentVar = tmp;                                                                                       \
        }                                                                                                                       \
        else res = 1;                                                                                                           \
    } while (0)                                                                                                                               

#define SEARCH_GLOBAL_AND_LOCAL(STR)                                                                                            \
    do {                                                                                                                        \
        Item_data *tmp = parserData->currentFunc;                                                                               \
        parserData->currentFunc = st_search( *parserData->globalSymTable, STR );                                                 \
        if (parserData->currentFunc == NULL) {                                                                                  \
            parserData->currentFunc = tmp;                                                                                      \
            tmp = parserData->currentVar;                                                                                       \
        }                                                                                                                       \
        parserData->currentVar = st_search( parserData->STStack.top->symTable, STR );                                           \
        if (parserData->currentVar == NULL) {                                                                                   \
            parserData->currentVar = tmp;                                                                                       \
            res = 0;                                                                                                            \
        }                                                                                                                       \
        else res = 1;                                                                                                           \
    } while (0)

#define SEARCH_ALL_LOCAL(STR)                                                                                                   \
    do {                                                                                                                        \
        SymTable_Stack_item *iterator = parserData->STStack.top;                                                                \
        Item_data *tmp = parserData->currentVar;                                                                                \
        while (iterator->depth > 0) {                                                                                           \
            parserData->currentVar = st_search( iterator->symTable, STR );                                                      \
            if (parserData->currentVar != NULL) break;                                                                          \
            iterator = iterator->nextItem;                                                                                      \
        }                                                                                                                       \
        if (parserData->currentVar == NULL) {                                                                                   \
            res = 0;                                                                                                            \
            parserData->currentVar = tmp;                                                                                       \
        } else {                                                                                                                \
            res = 1;                                                                                                            \
        }                                                                                                                       \
    } while (0)

#define ADD_GLOBAL(STR)                                                                                                         \
    do {                                                                                                                        \
        parserData->currentFunc = st_add_id( parserData->globalSymTable, STR );                                                \
        parserData->currentFunc->depth = parserData->currentDepth;                                                              \
    } while (0)

#define ADD_LOCAL(STR)                                                                                                          \
    do {                                                                                                                        \
        parserData->currentVar  = st_add_id( &parserData->STStack.top->symTable, STR );                                         \
        parserData->currentVar->depth = parserData->currentDepth;                                                               \
    } while (0)

typedef struct {

    char *id;                                   // identifier                   
    bool ifdec;                                 // if function is declared
    bool ifdef;                                 // if function is defined
    int depth;                                  // depth (symbol table lvl)
    Data_type type;                             // data type (for variable)
    Dynamic_string *inputTypes;                 // input types (for function)
    Dynamic_string *outputTypes;                // output types (for function)

} Item_data;

typedef struct Sym_table_elem {

    char *key;                                  // identifier
    Item_data data;                             // item data
    struct Sym_table_elem *leftItem;            // left child
    struct Sym_table_elem *rightItem;           // right child

} Sym_table_item;


/* typedef struct {

    Sym_table_item * rootItem;                 // pointer on root node

} Sym_table; */

typedef struct STSElem {

    int depth;
    Sym_table_item * symTable;
    struct STSElem *nextItem;

} SymTable_Stack_item;

typedef struct {

	SymTable_Stack_item *top;

} SymTable_Stack;

/**
 * @brief Initializes symbol table
 * @param symTable symbol table to initialize
 */
void st_init ( Sym_table_item **symTable );

/**
 * @brief Adds identifier to the table
 * @param symTable symbol table that id will be added to 
 * @param key identifier of function/variable
 */ 
Item_data *st_add_id ( Sym_table_item **symTable, char *key );
/**
 * @brief Adds input/output type to function
 * @param types dynamic string that represents i/o types
 * @param dataType data type to be added
 * @return Error code
 */
int st_add_param ( Dynamic_string *types, Keyword dataType );   

/**
 * @brief Adds data type to variable
 * @param token contains data type
 * @param item variable
 * @return Error code
 */
int st_add_type ( Token *token, Item_data *item );

/**
 * @brief Searches for identifier in given symbol table
 * @param rootItem Symbol table root item
 * @param key identifier
 * @return data or NULL in case nothing was found
 */
Item_data *st_search ( Sym_table_item *symTable, char *key );

/**
 * @brief Disposes entire symbol table
 * @param rootItem root item of symbol table
 */
void st_dispose ( Sym_table_item **symTable );

/**
 * @brief Debugging
 */
void st_print ( Sym_table_item *symTable );

/**
 * @brief Initializes symbol table stack
 * @param stack symbol table stack
 */
void sts_init ( SymTable_Stack *stack );

/**
 * @brief Pushes next item at the top of the stack
 * @param stack Symbol table stack
 * @param symTable Symbol table to be inserted
 * @param depth 
 */
Sym_table_item **sts_push ( SymTable_Stack *stack, int depth);

/**
 * @brief Removes symbol table item from the top of the stack
 * @param stack Symbol table stack
 */
void sts_pop ( SymTable_Stack *stack );

/**
 * @brief Disposes entire symbol table stack
 * @param stack Symbol table stack
 */
void sts_dispose ( SymTable_Stack_item *top );

#endif