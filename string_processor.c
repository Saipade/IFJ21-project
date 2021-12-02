/*

*/

#ifndef STRING_PROCESSOR_C
#define STRING_PROCESSOR_C

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "string_processor.h"
#include "errorslist.h"
#include "scanner.h"

bool ds_init ( Dynamic_string *str ) {

    if (!(str->str = malloc( DYNAMIC_STR_STARTING_MEM ))) return false; 
    
    str->length = 0;
    str->str[str->length] = '\0';
    str->size = DYNAMIC_STR_STARTING_MEM;

    return true;

}

bool ds_add_char ( Dynamic_string *str, char c ) {

    if (str->length + 1 >= str->size) {
        str->size = str->length + DYNAMIC_STR_INCREASE_MEM;
        if (!(str->str = realloc( str->str, str->size ))) return false;
    }

    str->str[str->length++] = c;
    str->str[str->length] = '\0';

    return true;

}

bool ds_copy ( Dynamic_string *src, Dynamic_string *dst ) {

    if (src->length >= dst->size) {
        if (!(dst->str = realloc( dst->str, src->length + 1 ))) return false;
        dst->size = src->length + 2;
    }
    
    strcpy( dst->str, src->str );
    dst->length = src->length;
    
    return true;

}

bool ds_add_chars ( Dynamic_string *dynamicStr, char *str) {

    if (dynamicStr->length + strlen(str) + 1 >= dynamicStr->size) {
        dynamicStr->size = dynamicStr->length + strlen(str) + DYNAMIC_STR_INCREASE_MEM;
        if (!(dynamicStr->str = realloc( dynamicStr->str, dynamicStr->size ))) return false;
    }

    strcat( dynamicStr->str, str );
    dynamicStr->length += strlen(str);
    dynamicStr->str[dynamicStr->length] = '\0';

    return true;

}

void ds_free ( Dynamic_string *str ) {

    free( str->str );

}



#endif