#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "string_processor.h"
#include "scanner.h"

// not found -> 1; found -> 0
#define SEARCH_GLOBAL(STR)                                                                                                      \
    do {                                                                                                                        \
        Item_data *tmp = parserData->currentFunc;                                                                               \
        parserData->currentFunc = st_search( parserData->symTable[0].rootItem, STR );                                           \
        if (parserData->currentFunc == NULL) {                                                                                  \
            res = 1;                                                                                                            \
            parserData->currentFunc = tmp;                                                                                      \
        }                                                                                                                       \
        else res = 0;                                                                                                           \
    } while (0)

#define SEARCH_GLOBAL_RHS(STR)                                                                                                  \
    do {                                                                                                                        \
        Item_data *tmp = parserData->rhsId;                                                                                     \
        parserData->rhsId = st_search( parserData->symTable[0].rootItem, STR );                                                 \
        if (parserData->rhsId == NULL) {                                                                                        \
            res = 1;                                                                                                            \
            parserData->rhsId = tmp;                                                                                            \
        }                                                                                                                       \
        else res = 0;                                                                                                           \
    } while (0)

#define SEARCH_LOCAL(STR)                                                                                                       \
    do {                                                                                                                        \
        Item_data *tmp = parserData->currentVar;                                                                                \
        parserData->currentVar = st_search( parserData->symTable[parserData->currentDepth].rootItem, STR );                     \
        if (parserData->currentVar == NULL) {                                                                                   \
            res = 1;                                                                                                            \
            parserData->currentVar = tmp;                                                                                       \
        }                                                                                                                       \
        else res = 0;                                                                                                           \
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
            res = 1;                                                                                                            \
        }                                                                                                                       \
        else res = 0;                                                                                                           \
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
            res = 1;                                                                                                            \
        }                                                                                                                       \
        else res = 0;                                                                                                           \
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
            res = 1;                                                                                                            \
            parserData->currentVar = tmp;                                                                                       \
        }                                                                                                                       \
        else res = 0;                                                                                                           \
    } while (0)

#define ADD_GLOBAL(STR)                                                                                                         \
        parserData->currentFunc = st_add_id( &parserData->symTable[0], STR )

#define ADD_LOCAL(STR)                                                                                                          \
        parserData->currentVar  = st_add_id( &parserData->symTable[parserData->currentDepth], STR )

typedef struct {

    char *id;                           // identifier                   
    bool ifdec;                         // if function is declared
    bool ifdef;                         // if function is defined
    Data_type type;                     // data type (for variable)
    Dynamic_string *inputTypes;         // input types (for function)
    Dynamic_string *outputTypes;        // output types (for function)

} Item_data;

typedef struct Sym_table_item {

    char *key;                          // identifier
    Item_data data;                     // item data
    struct Sym_table_item *leftItem;    // left child
    struct Sym_table_item *rightItem;   // right child

} *Sym_table_itemPtr;


typedef struct {

    Sym_table_itemPtr rootItem;         // pointer on root node

} Sym_table;

void st_init ( Sym_table *symTable );
Item_data *st_add_id ( Sym_table *symTable, char *key );
int st_add_param ( Dynamic_string *types, int dataType );   
Item_data *st_search (Sym_table_itemPtr rootItem, char *key);
int st_add_type ( Token *token, Item_data *item );
void st_dispose ( Sym_table_itemPtr *rootItem );
void st_insert ( Sym_table_itemPtr *rootItem, Sym_table_itemPtr newItem );
void st_free ( Sym_table *symTable );
void st_print ( Sym_table_itemPtr rootItem );

#endif