#include <stdlib.h>
#include <string.h>


#include "symtable.h"
#include "errorslist.h"
#include "scanner.h"
#include "parser.h"


bool st_init ( Sym_table *symTable ) {

    symTable->rootItem = NULL;

}

/**
 * Adds identifier to the table
 * @param symTable symbol table that id will be added to 
 * @param key key of identifier (str param of token)
 */ 
Item_data *st_add_id ( Sym_table *symTable, char *key ) {

    if (!symTable || !key) return NULL;
    
    struct Sym_table_item *newItem;
    if (!(newItem = malloc( sizeof( struct Sym_table_item ) ))) return NULL;
    
    newItem->key = malloc( strlen( key ) + 2 * sizeof( char ) );
    if (!newItem->key) {
        free( newItem );
        return NULL;
    }

    newItem->data.inputTypes = malloc( sizeof( Dynamic_string ) );
    if (!newItem->data.inputTypes) {
        free( newItem );
        free( newItem->key );
        return NULL;
    }

    if (!ds_init( newItem->data.inputTypes )) {
        free( newItem );
        free( newItem->key );
        free( newItem->data.inputTypes );
        return NULL;
    }

    newItem->data.outputTypes = malloc( sizeof( Dynamic_string ) );
    if (!newItem->data.outputTypes) {
        free( newItem );
        free( newItem->key );
        free( newItem->data.inputTypes );
        free( newItem->data.inputTypes->str );
        return NULL;
    }

    if (!ds_init( newItem->data.outputTypes )) {
        free( newItem );
        free( newItem->key );
        free( newItem->data.inputTypes );
        free( newItem->data.inputTypes->str );
        free( newItem->data.outputTypes);
        return NULL;
    }

    strcpy( newItem->key, key );
    newItem->data.ifdec = 1;
    newItem->data.ifdef = 0;

    st_insert( symTable->rootItem, newItem );

    return &newItem->data;

}

void st_insert ( Sym_table_itemPtr *rootItem, Sym_table_itemPtr newItem ) {

    if (newItem == NULL) return;

    if (rootItem == NULL) {
        
        *rootItem = newItem;
        (*rootItem)->leftItem = (*rootItem)->rightItem = NULL;
        
        
    }

    if (strcmp( newItem->key, (*rootItem)->key ) == 0) return;
    
    if (strcmp( newItem->key, (*rootItem)->key ) > 0) {

        if (!(*rootItem)->rightItem) (*rootItem)->rightItem = newItem;
        else st_insert( (*rootItem)->rightItem, newItem );

    } else if (strcmp( newItem->key, (*rootItem)->key ) < 0) {

        if (!(*rootItem)->leftItem) (*rootItem)->leftItem = newItem;
        else st_insert( (*rootItem)->leftItem, newItem );

    }

}


int st_add_value ( Parser_data *parserData ) {

    if (!parserData) return ERR_INTERNAL;

    switch (parserData->token.type) {

        case (TT_INT):

            if (parserData->currentId->type != IT_INT) return ERR_SEMANTIC_INCOP_TYPE;
            // TODO: DOPISAŤ

        break;
    }

}

int st_add_param ( Dynamic_string *types, int dataType ) {

    if (!dataType) return ERR_INTERNAL;

    switch (dataType) {

        case (KW_INTEGER):

            if (!ds_add_next( types, 'i' )) return ERR_INTERNAL;

            break;
        
        case (KW_NUMBER):

            if (!ds_add_next( types, 'd' )) return ERR_INTERNAL;

        break;

        case (KW_STRING):

            if (!ds_add_next( types, 's' )) return ERR_INTERNAL;

        break;

        case (KW_BOOLEAN):

            if (!ds_add_next( types, 'b' )) return ERR_INTERNAL;

        break;    

        default:

            return ERR_SYNTAX;

        break;
    
    }

    return 0;

}

int st_add_type ( Token *token, Item_data *item ) {

    if (!item || !token) return ERR_INTERNAL;

    switch (token->type) {

        case (KW_INTEGER):

            item->type = IT_INT;

        break;

        case (KW_NUMBER):

            item->type = IT_DOU;

        break;

        case (KW_STRING): 

            item->type = IT_STR;

        break;

        case (KW_BOOLEAN):

            item->type = IT_BOO;

        break;

        case (KW_NIL):

            item->type = IT_NIL;

        break;

        default:

            return ERR_SYNTAX;
        
        break;

    }

    return 0;

}

// strcmp : abdfa - abdfab = 98(b)     (strcmp( "abdfa", "abdfab" )) the longer the bigger

Item_data *st_search ( Sym_table_itemPtr rootItem, char *key ) {

    if (!rootItem || !key) return NULL;

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

void st_dispose ( Sym_table_itemPtr rootItem ) {

    if ( rootItem != NULL ) {

        st_dispose(&*rootItem->leftItem);

        st_dispose(&*rootItem->rightItem);
        
        free( rootItem->key );
        free( rootItem->data.inputTypes->str );
        free( rootItem->data.inputTypes );
        free( rootItem->data.outputTypes->str );
        free( rootItem->data.outputTypes );
        free( rootItem );

    }

}


void st_free ( Sym_table *symTable );


