#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "string_processor.h"
#include "scanner.h"

// found -> res = 1; not found -> res = 0
#define SEARCH_GLOBAL(STR)                                                                                                      \
    do {                                                                                                                        \
        Item_data *tmp = parserData->currentFunc;                                                                               \
        parserData->currentFunc = st_search( parserData->symTable[0].rootItem, STR );                                           \
        if (parserData->currentFunc == NULL) {                                                                                  \
            res = 0;                                                                                                            \
            parserData->currentFunc = tmp;                                                                                      \
        }                                                                                                                       \
        else res = 1;                                                                                                           \
    } while (0)

#define SEARCH_GLOBAL_RHS(STR)                                                                                                  \
    do {                                                                                                                        \
        Item_data *tmp = parserData->rhsId;                                                                                     \
        parserData->rhsId = st_search( parserData->symTable[0].rootItem, STR );                                                 \
        if (parserData->rhsId == NULL) {                                                                                        \
            res = 0;                                                                                                            \
            parserData->rhsId = tmp;                                                                                            \
        }                                                                                                                       \
        else res = 1;                                                                                                           \
    } while (0)

#define SEARCH_LOCAL(STR)                                                                                                       \
    do {                                                                                                                        \
        Item_data *tmp = parserData->currentVar;                                                                                \
        parserData->currentVar = st_search( parserData->symTable[parserData->currentDepth].rootItem, STR );                     \
        if (parserData->currentVar == NULL) {                                                                                   \
            res = 0;                                                                                                            \
            parserData->currentVar = tmp;                                                                                       \
        }                                                                                                                       \
        else res = 1;                                                                                                           \
    } while (0)                                                                                                                               

#define SEARCH_GLOBAL_AND_LOCAL(STR)                                                                                            \
    do {                                                                                                                        \
        Item_data *tmp = parserData->currentFunc;                                                                               \
        parserData->currentFunc = st_search( parserData->symTable[0].rootItem, STR );                                           \
        if (parserData->currentFunc == NULL) {                                                                                  \
            parserData->currentFunc = tmp;                                                                                      \
            tmp = parserData->currentVar;                                                                                       \
        }                                                                                                                       \
        parserData->currentVar = st_search( parserData->symTable[parserData->currentDepth].rootItem, STR );                     \
        if (parserData->currentVar == NULL) {                                                                                   \
            parserData->currentVar = tmp;                                                                                       \
            res = 0;                                                                                                            \
        }                                                                                                                       \
        else res = 1;                                                                                                           \
    } while (0)

#define SEARCH_ALL_LOCAL(STR)                                                                                                   \
    do {                                                                                                                        \
        Item_data *tmp = parserData->currentVar;                                                                                \
        for (int i = parserData->currentDepth; i > 0; i--) {                                                                    \
            parserData->currentVar = st_search( parserData->symTable[i].rootItem, STR );                                        \
            if (parserData->currentVar != NULL) break;                                                                          \
        }                                                                                                                       \
        if (parserData->currentVar == NULL) {                                                                                   \
            parserData->currentVar = tmp;                                                                                       \
            res = 0;                                                                                                            \
        }                                                                                                                       \
        else res = 1;                                                                                                           \
    } while (0)

#define SEARCH_EVERYWHERE(STR)                                                                                                  \
    do {                                                                                                                        \
        Item_data *tmp = parserData->currentFunc;                                                                               \
        parserData->currentFunc = st_search( parserData->symTable[0].rootItem, STR );                                           \
        if (parserData->currentFunc == NULL) {                                                                                  \
            parserData->currentFunc = tmp;                                                                                      \
            tmp = parserData->currentVar;                                                                                       \
        for (int i = parserData->currentDepth; i > 0; i--) {                                                                    \
            parserData->currentVar = st_search( parserData->symtable[i].rootItem, STR )                                         \
        if (parserData->currentVar != NULL) break                                                                               \
        }                                                                                                                       \
        if (parserData->currentVar == NULL) {                                                                                   \
            res = 0;                                                                                                            \
            parserData->currentVar = tmp;                                                                                       \
        }                                                                                                                       \
        else res = 1;                                                                                                           \
    } while (0)

#define ADD_GLOBAL(STR)                                                                                                         \
    do {                                                                                                                        \
        parserData->currentFunc = st_add_id( &parserData->symTable[0], STR );                                                   \
        parserData->currentFunc->depth = parserData->currentDepth;                                                              \
    } while (0)

#define ADD_LOCAL(STR)                                                                                                          \
    do {                                                                                                                        \
        parserData->currentVar  = st_add_id( &parserData->symTable[parserData->currentDepth], STR );                            \
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

typedef struct Sym_table_item {

    char *key;                                  // identifier
    Item_data data;                             // item data
    struct Sym_table_item *leftItem;            // left child
    struct Sym_table_item *rightItem;           // right child

} *Sym_table_itemPtr;


typedef struct {

    Sym_table_itemPtr rootItem;                 // pointer on root node

} Sym_table;

typedef struct STSElem {

    Sym_table *table;
    struct STSElem *nextItem;

} SymTable_Stack_item;

typedef struct {
    
    SymTable_Stack_item *top;

} SymTable_Stack;

/**
 * @brief Initializes symbol table
 * @param symTable symbol table to initialize
 */
void st_init ( Sym_table *symTable );

/**
 * @brief Adds identifier to the table
 * @param symTable symbol table that id will be added to 
 * @param key identifier of function/variable
 */ 
Item_data *st_add_id ( Sym_table *symTable, char *key );
int st_add_param ( Dynamic_string *types, Keyword dataType );   
Item_data *st_search (Sym_table_itemPtr rootItem, char *key);
int st_add_type ( Token *token, Item_data *item );
void st_dispose ( Sym_table_itemPtr *rootItem );
int st_insert ( Sym_table_itemPtr *rootItem, Sym_table_itemPtr newItem );
void st_free ( Sym_table *symTable );
void st_print ( Sym_table_itemPtr rootItem );

#endif