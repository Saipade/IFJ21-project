/*

*/

#include <stdlib.h>

#include "scanner.c"
#include "errorslist.h"
#include "string_processor.c"
#include "parser.c"

FILE *srcF;

/* typedef struct {

    Token token;

} Parser_data;

int check_prologue ( Parser_data *parserData) {

    int tmp;
    if (tmp = get_next_token( &parserData->token ) != SCAN_OK) return tmp;
    printf( "(%d %d)\n", parserData->token.type, parserData->token.attribute.keyword);
    if (parserData->token.type != TT_KEY) return ERR_SYNTAX;
    if (tmp = get_next_token( &parserData->token )) return tmp;
    if (strcmp(parserData->token.attribute.string->str, "ifj21")) return ERR_SYNTAX;
    printf( "(%d %s)\n", parserData->token.type, parserData->token.attribute.string->str);
    return true;
 
}

bool parser_data_init ( Parser_data *parserData ) {

    
    return true;

} */



int main( int argc, char *argv[] ) {

    srcF = fopen( argv[1], "r");
    
    parse (  );


    return 0;
}