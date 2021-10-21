/*

*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#include "string_processor.h"

bool ds_init ( Dynamic_string *cStr ) {

    if (!(cStr->str = (char *) malloc( DYNAMIC_STR_STARTING_MEM ))) return false; 
    
    cStr->length = 0;
    cStr->str[cStr->length] = '\0';
    cStr->size = DYNAMIC_STR_STARTING_MEM;

    return true;

}

bool ds_add_next ( Dynamic_string *cStr, char c ) {

    if (cStr->length + 2 >= cStr->size) {
        cStr->size = cStr->length + DYNAMIC_STR_INCREASE_MEM;
        if (!(cStr->str = (char*) realloc( cStr->str, cStr->size ))) return false;
    }

    cStr->str[cStr->length++] = c;
    cStr->str[cStr->length] = '\0';

    return true;

}

bool ds_copy ( Dynamic_string *src, Dynamic_string *dst ) {

    if (src->length >= dst->size) {
        if (!(dst->str = (char*) realloc( dst->str, src->length + 2 ))) return false;
        dst->size = src->length + 2;
    }
    
    strcpy( dst->str, src->str );
    dst->length = src->length;
    
    return true;

}

void ds_mem_free ( Dynamic_string *cStr ) {

    free( cStr->str );

}
