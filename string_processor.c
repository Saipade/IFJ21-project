/*

*/

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

bool ds_add_next ( Dynamic_string *str, char c ) {

    if (str->length + 2 >= str->size) {
        str->size = str->length + DYNAMIC_STR_INCREASE_MEM;
        if (!(str->str = realloc( str->str, str->size ))) return false;
    }

    str->str[str->length++] = c;
    str->str[str->length] = '\0';

    return true;

}

bool ds_copy ( Dynamic_string *src, Dynamic_string *dst ) {

    if (src->length >= dst->size) {
        if (!(dst->str = realloc( dst->str, src->length + 2 ))) return false;
        dst->size = src->length + 2;
    }
    
    strcpy( dst->str, src->str );
    dst->length = src->length;
    
    return true;

}

bool ds_add_str ( Dynamic_string *dynamicStr, char *str) {

    if (dynamicStr->length + strlen(str) + 2 >= dynamicStr->size) {
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

int _integer_or_number ( Dynamic_string *str, Token *token ) {

    char *ptr;
    
    if (token->type == TT_INT) {

        int tmp = (int) strtol( str->str, &ptr, 10 );
        token->attribute.integer = tmp;
        ds_free( str );
        return SCAN_OK;

    } 
    
    else if (token->type == TT_DOU) {

        double tmp = strtod( str->str, &ptr );
        token->attribute.floating = tmp;
        ds_free( str );
        return SCAN_OK;
        
    }

}

int _keyword_or_id ( Dynamic_string *str, Token *token ) {

    if      (strcmp( str->str, "integer" ) == 0) token->attribute.keyword = KW_INTEGER;
    else if (strcmp( str->str, "number" ) == 0) token->attribute.keyword = KW_NUMBER;
    else if (strcmp( str->str, "string" ) == 0) token->attribute.keyword = KW_STRING;
    else if (strcmp( str->str, "boolean" ) == 0) token->attribute.keyword = KW_BOOLEAN;
    else if (strcmp( str->str, "nil" ) == 0) token->attribute.keyword = KW_NIL;
    else if (strcmp( str->str, "do" ) == 0) token->attribute.keyword = KW_DO;
    else if (strcmp( str->str, "else" ) == 0) token->attribute.keyword = KW_ELSE;
    else if (strcmp( str->str, "end" ) == 0) token->attribute.keyword = KW_END;
    else if (strcmp( str->str, "function" ) == 0) token->attribute.keyword = KW_FUNCTION;
    else if (strcmp( str->str, "global" ) == 0) token->attribute.keyword = KW_GLOBAL;
    else if (strcmp( str->str, "if" ) == 0) token->attribute.keyword = KW_IF;
    else if (strcmp( str->str, "local" ) == 0) token->attribute.keyword = KW_LOCAL;
    else if (strcmp( str->str, "require" ) == 0) token->attribute.keyword = KW_REQUIRE;
    else if (strcmp( str->str, "return" ) == 0) token->attribute.keyword = KW_RETURN;
    else if (strcmp( str->str, "then" ) == 0) token->attribute.keyword = KW_THEN;
    else if (strcmp( str->str, "while" ) == 0) token->attribute.keyword = KW_WHILE;

    else if (strcmp( str->str, "reads" ) == 0) token->attribute.keyword = KW_READS;
    else if (strcmp( str->str, "raedi" ) == 0) token->attribute.keyword = KW_READI;
    else if (strcmp( str->str, "readn" ) == 0) token->attribute.keyword = KW_READN;
    else if (strcmp( str->str, "write" ) == 0) token->attribute.keyword = KW_WRITE;
    else if (strcmp( str->str, "tointeger" ) == 0) token->attribute.keyword = KW_TOINTEGER;
    else if (strcmp( str->str, "substr" ) == 0) token->attribute.keyword = KW_SUBSTR;
    else if (strcmp( str->str, "ord" ) == 0) token->attribute.keyword = KW_ORD;
    else if (strcmp( str->str, "chr" ) == 0) token->attribute.keyword = KW_CHR;
    
    else token->type = TT_IDE;

    if (token->type == TT_IDE) {
        
        if (!ds_copy( str, token->attribute.string )) {
            ds_free( str );
            return ERR_INTERNAL;
        }

        ds_free( str );
        return SCAN_OK;
        
    }
    
    else {

        token->type = TT_KEY;
        ds_free( str );
        return SCAN_OK;

    }

}