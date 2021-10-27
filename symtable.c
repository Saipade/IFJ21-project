#include <stdlib.h>
#include <string.h>


#include "symtable.h"
#include "errorslist.h"
#include "scanner.h"


bool st_init ( Sym_table *symTable ) {

    if (!symTable) return false;

    if (!(symTable = malloc( sizeof( symTable ) ))) return false;
    symTable->rootItem = NULL;

}

/**
 * Adds identifier to the table
 * @param symTable symbol table that id will be added to 
 * @param key key of identifier (str param of token)
 */ 
Item_data *st_add_id ( Sym_table *symTable, char *key ) {

    if (!symTable || !key) return NULL;

    Sym_table_item *newItem = malloc( sizeof(Sym_table_item) );
    if (!newItem) return NULL;
    
    newItem->key = malloc( strlen( key ) + 2 * sizeof( char ) );
    if (!newItem->key) {
        free( newItem );
        return NULL;
    }
    strcpy( newItem->key, key );

    newItem->data.types = malloc( sizeof( Dynamic_string ) );
    if (!newItem->data.type) {
        free( newItem );
        free( newItem->key );
        return NULL;
    }

    ds_init( newItem->data.types );
    if (!newItem->data.types) {
        free( newItem );
        free( newItem->key );
        free( newItem->data.types );
        return NULL;
    }

    st_insert( symTable->rootItem, newItem );

    return &newItem->data;

}

int st_add_type ( Parser_data *parserData ) {

    if (!parserData) return ERR_INTERNAL;

    switch (parserData->token.attribute.keyword) {

        case (KW_INTEGER):

            parserData->currentId->type = IT_INT;

        break;

        case (KW_NUMBER):

            parserData->currentId->type = IT_DOU;

        break;

        case (KW_STRING): 

            parserData->currentId->type = IT_STR;

        break;

        case (KW_BOOL):

            parserData->currentId->type = IT_BOO;

        break;

        case (KW_NIL):

            parserData->currentId->type = IT_NIL;

        break;

        default:

            return ERR_SYNTAX;
        
        break;

    }

    return 0;

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

int st_add_param ( Parser_data *parserData ) {

    if (!parserData) return ERR_INTERNAL;

    switch (parserData->token.attribute.keyword) {

        case (KW_INTEGER):

            if (!ds_add_next( parserData->currentId->types, 'i' )) return ERR_INTERNAL;

            break;
        
        case (KW_NUMBER):

            if (!ds_add_next( parserData->currentId->types, 'd' )) return ERR_INTERNAL;

        break;

        case (KW_STRING):

            if (!ds_add_next( parserData->currentId->types, 's' )) return ERR_INTERNAL;

        break;
        
        default:

            return ERR_SYNTAX;

        break;
    
    }

    return 0;

}

// strcmp : abdfa - abdfab = 98(b)     (strcmp( "abdfa", "abdfab" )) the longer the bigger

/* Sym_table_item *st_search ( Sym_table_item *rootItem, char *key ) {

    if (!rootItem || !key) return 0;

    if (strcmp( key, rootItem->key ) == 0) {
        return rootItem;
    }
    if (strcmp( key, rootItem->key ) > 0) {
        return st_search( rootItem->rightItem, key );
    }
    else if (strcmp( key, rootItem->key ) < 0) {
        return st_search( rootItem->leftItem, key );
    }

} */

Sym_table_item *st_insert ( Sym_table_item *rootItem, Sym_table_item *newItem ) {

    if (rootItem == NULL) {
        rootItem = newItem;
    
    if (strcmp( newItem->key, rootItem->key ) >= 0) {

        if (!rootItem->rightItem) rootItem->rightItem = newItem;
        else st_insert( rootItem->rightItem, newItem );

    } else if (strcmp( newItem->key, rootItem->key ) < 0) {

        if (!rootItem->leftItem) rootItem->leftItem = newItem;
        else st_insert( rootItem->leftItem, newItem );

    }

}


void st_free ( Sym_table *symTable );


