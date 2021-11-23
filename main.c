/*

*/
#include "string_processor.h"
#include "errorslist.h"
#include "parser.h"

#include <stdlib.h>



int main ( int argc, char *argv[] ) {

    FILE *srcF;

    srcF = stdin;

    _source_file( srcF );

    parse (  );

    return 0;

}