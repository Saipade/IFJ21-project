



#include "symtable.h"
#include "errorslist.h"
#include "scanner.h"
#include "parser.h"

#include <stdlib.h>
#include <string.h>


void st_init ( Sym_table *symTable ) {

    symTable->rootItem = NULL;


}

/**
 * Adds identifier to the table
 * @param symTable symbol table that id will be added to 
 * @param key identifier of function/variable
 */ 
Item_data *st_add_id ( Sym_table *symTable, char *key ) {

    if (!symTable || !key) return NULL;
    
    struct Sym_table_item *newItem;
    if (!(newItem = malloc( sizeof( struct Sym_table_item ) ))) {
        exit(99);
        return NULL;
    }

    if (!(newItem->key = malloc( strlen( key ) * sizeof( char ) + 2 ))) {
        exit(99);
    }
    
    if (!(newItem->data.id = malloc( strlen( key ) * sizeof( char ) + 2 ))) {
        exit(99);
    }

    if (!(newItem->data.inputTypes = malloc( sizeof( Dynamic_string ) ))) {
        exit(99);
    }

    if (!ds_init( newItem->data.inputTypes )) {
        exit(99);
    }

    newItem->data.outputTypes = malloc( sizeof( Dynamic_string ) );
    if (!newItem->data.outputTypes) {
        exit(99);
    }

    if (!ds_init( newItem->data.outputTypes )) {
        exit(99);
    }

    strcpy( newItem->key, key );
    strcpy( newItem->data.id, newItem->key);
    newItem->data.ifdec = 1;
    newItem->data.ifdef = 0;
    newItem->leftItem = newItem->rightItem = NULL;

    st_insert( &(symTable->rootItem), newItem );

    return &newItem->data;

}

void st_insert ( Sym_table_itemPtr *rootItem, Sym_table_itemPtr newItem ) {

    if (newItem == NULL) return;

    if (rootItem == NULL || (*rootItem) == NULL) {

        //printf("%s\n", newItem->key);
        
        *rootItem = newItem;
        
    }

    if (strcmp( newItem->key, (*rootItem)->key ) == 0) return;
    
    if (strcmp( newItem->key, (*rootItem)->key ) > 0) {

        st_insert( &((*rootItem)->rightItem), newItem );

    } else if (strcmp( newItem->key, (*rootItem)->key ) < 0) {

        st_insert( &((*rootItem)->leftItem), newItem );

    }

}

int st_add_param ( Dynamic_string *types, int dataType ) {

    if (!types) return ERR_INTERNAL;
    
    switch (dataType) {

        case (KW_INTEGER):

            if (!ds_add_char( types, '0' )) return ERR_INTERNAL;

            break;
        
        case (KW_NUMBER):

            if (!ds_add_char( types, '1' )) return ERR_INTERNAL;

        break;

        case (KW_STRING):

            if (!ds_add_char( types, '2' )) return ERR_INTERNAL;

        break;

        /* case (KW_BOOLEAN):

            if (!ds_add_char( types, 'b' )) return ERR_INTERNAL;

        break;     */

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

// strcmp : abdfa - abdfab = 98(b)     (strcmp( "abdfa", "abdfab" )) the longer the bigger

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

        st_dispose( &(*rootItem)->leftItem  );

        st_dispose( &(*rootItem)->rightItem );
        
        free( (*rootItem)->key );
        free( (*rootItem)->data.inputTypes->str );
        free( (*rootItem)->data.inputTypes );
        free( (*rootItem)->data.outputTypes->str );
        free( (*rootItem)->data.outputTypes );
        free( (*rootItem) );

        (*rootItem)->key = NULL;
        (*rootItem)->data.inputTypes->str = NULL;
        (*rootItem)->data.inputTypes = NULL;
        (*rootItem)->data.outputTypes->str = NULL;
        (*rootItem)->data.outputTypes = NULL;
        (*rootItem) = NULL;

    }

}


void st_free ( Sym_table *symTable );

// for debugging reasons
void st_print ( Sym_table_itemPtr rootItem ) {

    printf("%s\n", rootItem->key);
    if (rootItem->leftItem) st_print( rootItem->leftItem );
    if (rootItem->rightItem) st_print( rootItem->rightItem );

}
