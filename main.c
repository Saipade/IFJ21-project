/**
 * PROJECT: Implementation of imperative IFJ21 programming language compiler
 * 
 * AUTHOR(S): Maksim Tikhonov (xtikho00)
 *            Sadovskyi Dmytro (xsadov06)
 *            Galliamov Eduard (xgalli01)
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