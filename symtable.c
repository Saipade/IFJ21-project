/**
 * PROJECT: Implementation of imperative IFJ21 programming language compiler
 * PART: Symbol table
 * 
 * AUTHOR(S): Sadovskyi Dmytro (xsadov06)
 *            Galliamov Eduard (xgalli01)
 */

#include "symtable.h"
#include "errorslist.h"
#include "scanner.h"
#include "string_processor.h"

#include <stdlib.h>
#include <string.h>

/* .......................................... Symbol table .......................................... */

void st_init ( Sym_table_itemPtr *symTable ) {

    *symTable = NULL;

}

Item_data *st_add_id ( Sym_table_itemPtr *symTable, char *key ) {

    if (!symTable || !key) return NULL;
    
    Sym_table_itemPtr newItem;
    newItem = malloc( sizeof( struct Sym_table_item ) );
    if (!newItem) exit( ERR_INTERNAL );

    // id name
    newItem->key = malloc( (strlen( key ) + 1) * sizeof( char ) );
    if (!newItem->key) exit( ERR_INTERNAL );
    strcpy( newItem->key, key );
    newItem->data.id = newItem->key;

    // input params initialization                                                       
    newItem->data.inputTypes = malloc( sizeof( Dynamic_string ));
    if (!newItem->data.inputTypes) exit( ERR_INTERNAL );
    if (!ds_init( newItem->data.inputTypes )) exit( ERR_INTERNAL );
    
    // output types initialization
    newItem->data.outputTypes = malloc( sizeof( Dynamic_string ));
    if (!newItem->data.outputTypes) exit( ERR_INTERNAL );
    if (!ds_init( newItem->data.outputTypes )) exit( ERR_INTERNAL );

    newItem->leftItem = newItem->rightItem = NULL;
    newItem->data.ifdec = 0;
    newItem->data.ifdef = 0;
    // insert initialized item to symbol table
    st_insert( symTable, newItem );

    return &newItem->data;

}

int st_insert ( Sym_table_itemPtr *symTable, Sym_table_itemPtr newItem ) {
    
    if (symTable == NULL) return ERR_INTERNAL;
    
    if (!(*symTable)) {

        *symTable = newItem;
        
    }

    else if (!strcmp( newItem->key, (*symTable)->key )) exit( ERR_SEMANTIC_UNDEF_VAR );
    
    else if (strcmp( newItem->key, (*symTable)->key ) > 0) {
        
        st_insert( &(*symTable)->rightItem, newItem );

    } 
    
    else if (strcmp( newItem->key, (*symTable)->key ) < 0) {

        st_insert( &(*symTable)->leftItem, newItem );

    }

}

int st_add_param ( Dynamic_string *types, Keyword dataType ) {

    if (!types) return ERR_INTERNAL;
    
    switch (dataType) {

        case (KW_INTEGER):
            if (!ds_add_char( types, 'i' )) exit( ERR_INTERNAL );
        break;
        
        case (KW_NUMBER):
            if (!ds_add_char( types, 'n' )) exit( ERR_INTERNAL );
        break;

        case (KW_STRING):
            if (!ds_add_char( types, 's' )) exit( ERR_INTERNAL );
        break;

        /* case (KW_BOOLEAN): until better times
            if (!ds_add_char( types, 'b' )) return ERR_INTERNAL;*/

        case (ANY):
            if (!ds_add_char( types, 'a' )) exit( ERR_INTERNAL );
        break;

        default:
            return ERR_SYNTAX;
    
    }

    return 0;

}

int st_add_type ( Token *token, Item_data *item ) {

    if (!item || !token) return ERR_INTERNAL;

    switch (token->attribute.keyword) {

        case (KW_INTEGER):
            item->type = T_INT;
        break;

        case (KW_NUMBER):
            item->type = T_NUM;
        break;

        case (KW_STRING): 
            item->type = T_STR;
        break;

        /* case (KW_BOOLEAN):
            item->type = T_BOO;*/

        default:
            return ERR_SYNTAX;

    }

    return 0;

}

Item_data *st_search ( Sym_table_itemPtr symTable, char *key ) {

    if (!symTable) return NULL;

    if (!strcmp( key, symTable->key )) {
            
        return &symTable->data;

    }
    
    else if (strcmp( key, symTable->key ) > 0) {

        return st_search( symTable->rightItem, key );

    }

    else if (strcmp( key, symTable->key ) < 0) {

        return st_search( symTable->leftItem, key );

    }

}  

void st_dispose ( Sym_table_itemPtr *symTable ) {

    if (!(*symTable)) return;

    st_dispose( &(*symTable)->leftItem );
    st_dispose( &(*symTable)->rightItem );
    
    ds_free( (*symTable)->data.inputTypes );
    free( (*symTable)->data.inputTypes );
    ds_free( (*symTable)->data.outputTypes );
    free( (*symTable)->data.outputTypes );
    free( (*symTable)->key );
    free( (*symTable) );

    (*symTable)->data.inputTypes->str = NULL;
    (*symTable)->data.inputTypes = NULL;
    (*symTable)->data.outputTypes->str = NULL;
    (*symTable)->data.outputTypes = NULL;
    (*symTable)->key = NULL;
    (*symTable) = NULL;
    
}

// for debugging reasons
void st_print ( Sym_table_itemPtr *symTable ) {
    //printf("hi\n");
    if (*symTable) {
        printf( "%s \n", (*symTable)->key );
        if ((*symTable)->leftItem) st_print( &(*symTable)->leftItem );
        if ((*symTable)->rightItem) st_print( &(*symTable)->rightItem );
    }

}

/* .......................................... Symbol table stack .......................................... */

void sts_init ( SymTable_Stack *stack ) {

    stack = malloc( sizeof (SymTable_Stack) );
    if (stack) stack->top = NULL;

}

Sym_table_itemPtr *sts_push ( SymTable_Stack *stack, int depth ) {

    if (!stack) return NULL;

    SymTable_Stack_item *newItem = malloc( sizeof( SymTable_Stack_item ));
    if (!newItem) return NULL;

    newItem->symTable = malloc( sizeof( Sym_table_itemPtr ));
    if (!newItem->symTable) return NULL;
    st_init( newItem->symTable );

    newItem->nextItem = stack->top;
    newItem->depth = depth;
    stack->top = newItem;

    return newItem->symTable;

}

void sts_pop ( SymTable_Stack *stack ) {

    if (stack->top) {

        st_dispose( stack->top->symTable );
        free( stack->top->symTable );
        stack->top->symTable = NULL;

        SymTable_Stack_item *toPop = stack->top;
        stack->top = stack->top->nextItem;

        free( toPop );
        stack->top = NULL;

    }

}