/*

*/

#include <stdlib.h>

#include "scanner.c"
#include "errorslist.h"
#include "string_processor.c"

FILE *srcF;

int main(int argc, char *argv[]) {

    srcF = fopen( argv[1], "r+" );
    
    Token token;
    Token *ptr = &token;

    Dynamic_string dynamicString;
    if (!ds_init( &dynamicString )) return ERR_INTERNAL;
    set_dynamic_string( &dynamicString );
                                                // что тестируем: scanner.c
    int err = get_next_token( &token );         // тестировать ВСЕ виды токенов (создаешь текстовый файл в папке с тестируемым типом токена) -> пишешь main.exe *filename*.txt
                                                // особенно тестировать escape sequence (см. задание проекта)
    printf("(%d %d)\n", token.type, err);       // затем выписываешь его тестируемые хар-ки (см. структуру токена в scanner.h)
    

    return 0;
}