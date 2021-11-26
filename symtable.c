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

void st_init ( Sym_table *symTable ) {

    symTable->rootItem = NULL;

}

Item_data *st_add_id ( Sym_table *symTable, char *key ) {

    if (!symTable || !key) return NULL;
    
    struct Sym_table_item *newItem;
    if (!(newItem = malloc( sizeof( struct Sym_table_item ) ))) {
        exit( ERR_INTERNAL );
        return NULL;
    }

    if (!(newItem->key = malloc( strlen( key ) * sizeof( char ) + 2 ))) {
        exit( ERR_INTERNAL );
    }
    
    if (!(newItem->data.id = malloc( strlen( key ) * sizeof( char ) + 2 ))) {
        exit( ERR_INTERNAL );
    }

    if (!(newItem->data.inputTypes = malloc( sizeof( Dynamic_string ) ))) {
        exit( ERR_INTERNAL );
    }

    if (!ds_init( newItem->data.inputTypes )) {
        exit( ERR_INTERNAL );
    }

    newItem->data.outputTypes = malloc( sizeof( Dynamic_string ) );
    if (!newItem->data.outputTypes) {
        exit( ERR_INTERNAL );
    }

    if (!ds_init( newItem->data.outputTypes )) {
        exit( ERR_INTERNAL );
    }

    strcpy( newItem->key, key );
    strcpy( newItem->data.id, newItem->key);
    newItem->leftItem = newItem->rightItem = NULL;
    newItem->data.ifdec = 0;
    newItem->data.ifdef = 0;

    st_insert( &(symTable->rootItem), newItem );

    return &newItem->data;

}

int st_insert ( Sym_table_itemPtr *rootItem, Sym_table_itemPtr newItem ) {
    
    if (newItem == NULL) return ERR_INTERNAL;
    
    if (rootItem == NULL || (*rootItem) == NULL) {

        *rootItem = newItem;
        
    }

    else if (!strcmp( newItem->key, (*rootItem)->key )) exit( ERR_SEMANTIC_UNDEF_VAR );
    
    else if (strcmp( newItem->key, (*rootItem)->key ) > 0) {
        
        st_insert( &((*rootItem)->rightItem), newItem );

    } 
    
    else if (strcmp( newItem->key, (*rootItem)->key ) < 0) {

        st_insert( &((*rootItem)->leftItem), newItem );

    }

}

int st_add_param ( Dynamic_string *types, Keyword dataType ) {

    if (!types) return ERR_INTERNAL;
    
    switch (dataType) {

        case (KW_INTEGER):

            if (!ds_add_char( types, 'i' )) exit(ERR_INTERNAL);

            break;
        
        case (KW_NUMBER):

            if (!ds_add_char( types, 'n' )) exit(ERR_INTERNAL);

        break;

        case (KW_STRING):

            if (!ds_add_char( types, 's' )) exit(ERR_INTERNAL);

        break;

        /* case (KW_BOOLEAN):

            if (!ds_add_char( types, 'b' )) return ERR_INTERNAL;

        break;     */

        case (ANY):

            if (!ds_add_char( types, 'a' )) exit(ERR_INTERNAL);

        break;

        default:

            return ERR_SYNTAX;

        break;
    
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

            item->type = T_BOO;

        break; */

        case (KW_NIL):

            item->type = T_NIL;

        break;

        default:

            return ERR_SYNTAX;
        
        break;

    }

    return 0;

}

Item_data *st_search ( Sym_table_itemPtr rootItem, char *key ) {
    
    if (rootItem == NULL) return NULL;

    if (strcmp( key, rootItem->key ) == 0) {
            
        return &rootItem->data;

    }
    
    else if (strcmp( key, rootItem->key ) > 0) {
        if (rootItem->rightItem)
        return st_search( rootItem->rightItem, key );

    }

    else if (strcmp( key, rootItem->key ) < 0) {
        if (rootItem->leftItem)
        return st_search( rootItem->leftItem, key );

    }

}  

void st_dispose ( Sym_table_itemPtr *rootItem ) {
    
    if ( rootItem != NULL ) {

        if ((*rootItem)->leftItem) st_dispose( &(*rootItem)->leftItem  );

        if ((*rootItem)->rightItem) st_dispose( &(*rootItem)->rightItem );
        
        free( (*rootItem)->key );
        free( (*rootItem)->data.inputTypes->str );
        free( (*rootItem)->data.inputTypes );
        free( (*rootItem)->data.outputTypes->str );
        free( (*rootItem)->data.outputTypes );

        (*rootItem)->key = NULL;
        (*rootItem)->data.inputTypes->str = NULL;
        (*rootItem)->data.inputTypes = NULL;
        (*rootItem)->data.outputTypes->str = NULL;
        (*rootItem)->data.outputTypes = NULL;

    } 

}


void st_free ( Sym_table *symTable );

// for debugging reasons
void st_print ( Sym_table_itemPtr rootItem ) {

    printf("%s\n", rootItem->key);
    if (rootItem->leftItem) st_print( rootItem->leftItem );
    if (rootItem->rightItem) st_print( rootItem->rightItem );

}
