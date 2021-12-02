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
// TO DELETE
void *safemalloc( size_t _Size ){
	void *ret = malloc( _Size );
	if ( ret == NULL ) {
		exit(99);
	}
	return ret;
}

void st_init ( Sym_table_item **symTable ) {

    *symTable = NULL;

}

Item_data *st_add_id ( Sym_table_item **symTable, char *key ) {
    
    if (!(*symTable)) {
        
        *symTable = safemalloc( sizeof( Sym_table_item ) );
        if (!*symTable) exit( ERR_INTERNAL );
        
        // name
        (*symTable)->key = malloc( (strlen( key ) + 1) * sizeof( char ) );
        if (!(*symTable)->key) exit( ERR_INTERNAL );
        strcpy( (*symTable)->key, key );
        (*symTable)->data.id = (*symTable)->key;
        
        // input params (dynamic string) initialization                                                       
        (*symTable)->data.inputTypes = malloc( sizeof( Dynamic_string ));
        if (!(*symTable)->data.inputTypes) exit( ERR_INTERNAL );
        if (!ds_init( (*symTable)->data.inputTypes )) exit( ERR_INTERNAL );
        
        // output types (dynamic string) initialization
        (*symTable)->data.outputTypes = malloc( sizeof( Dynamic_string ));
        if (!(*symTable)->data.outputTypes) exit( ERR_INTERNAL );
        if (!ds_init( (*symTable)->data.outputTypes )) exit( ERR_INTERNAL );

        (*symTable)->leftItem = NULL;
        (*symTable)->rightItem = NULL;
        (*symTable)->data.ifdec = 0;
        (*symTable)->data.ifdef = 0;

        return &(*symTable)->data;

    }

    else if (!strcmp( key, (*symTable)->key )) exit( ERR_SEMANTIC_UNDEF_VAR );
    
    else if (strcmp( key, (*symTable)->key ) > 0) {
        
        st_add_id( &(*symTable)->rightItem, key );

    } 
    
    else if (strcmp( key, (*symTable)->key ) < 0) {

        st_add_id( &(*symTable)->leftItem, key );

    }

}

void st_dispose ( Sym_table_item **symTable ) {

    if (!(*symTable)) return;

    st_dispose( &(*symTable)->leftItem );
    st_dispose( &(*symTable)->rightItem );

    Sym_table_item *tmp = *symTable;
    ds_free( tmp->data.inputTypes );
    free( tmp->data.inputTypes );
    ds_free( tmp->data.outputTypes );
    free( tmp->data.outputTypes );
    free( tmp->key );
    free( tmp );

    tmp = NULL;
    
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

        default:
            return ERR_SYNTAX;

    }

    return 0;

}

Item_data *st_search ( Sym_table_item *symTable, char *key ) {

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

// for debugging reasons
void st_print ( Sym_table_item *symTable ) {
    
    if (symTable) {
        printf( "%s \n", symTable->key );
        if (symTable->leftItem) st_print( symTable->leftItem );
        if (symTable->rightItem) st_print( symTable->rightItem );
    }

}

/* .......................................... Symbol table stack .......................................... */

void sts_init ( SymTable_Stack *stack ) {

    if (stack) stack->top = NULL;

}

Sym_table_item **sts_push ( SymTable_Stack *stack, int depth ) {

    if (!stack) return NULL;

    SymTable_Stack_item *newItem = safemalloc( sizeof( SymTable_Stack_item ));
    if (!newItem) exit( ERR_INTERNAL );

    st_init( &newItem->symTable );

    newItem->nextItem = stack->top;
    stack->top = newItem;
    newItem->depth = depth;

    return &newItem->symTable;

}

void sts_pop ( SymTable_Stack *stack ) {

    if (stack->top) {

        SymTable_Stack_item *toPop = stack->top;
        stack->top = stack->top->nextItem;

        st_dispose( &toPop->symTable );
        toPop->symTable = NULL;

        free( toPop );

    }

}

void sts_dispose ( SymTable_Stack_item *top ) {
    
    if (!top) return;
    // recursion
    sts_dispose( top->nextItem );
    // symbol table dispose
    st_dispose( &top->symTable );
    top->symTable = NULL;
    free( top );
    top = NULL;

}