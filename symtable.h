#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "string_processor.h"

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

bool st_init ( Sym_table *symTable );
Item_data *st_search (Sym_table_itemPtr rootItem, char *key);
void st_dispose ( Sym_table_itemPtr *rootItem );
void st_insert ( Sym_table_itemPtr *rootItem, Sym_table_itemPtr newItem );
void st_print ( Sym_table_itemPtr rootItem );

#endif