#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "string_processor.h"

typedef enum {

    IT_INT,
    IT_DOU,
    IT_STR,
    IT_BOO,
    IT_NIL,
    IT_ND,

} Item_dataType;

typedef struct {

    Item_dataType type;     //
    bool scale;             // 0 = global; 1 = local
    bool ifdef;             // for functions: 0 = not defined, 1 = defined
    Dynamic_string *types;  // for function declarations: "input types#output types"
                            // types are represented so: i - int, d(ouble) - num, s - str, b - bool
} Item_data;

typedef struct {

    char *key;
    Item_data data;
    struct Sym_table_item *leftItem;
    struct Sym_table_item *rightItem;

} Sym_table_item;

typedef struct {

    Sym_table_item *rootItem;

} Sym_table;

bool st_init ( Sym_table *symTable );
Sym_table *st_insert ( Sym_table_item *rootItem, Sym_table_item *newItem );


#endif
