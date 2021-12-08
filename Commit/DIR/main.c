/**
 * PROJECT: Implementation of imperative IFJ21 programming language compiler
 * 
 * AUTHOR(S): Maksim Tikhonov (xtikho00)
 *            Sadovskyi Dmytro (xsadov06)
 */

#include "string_processor.h"
#include "errorslist.h"
#include "parser.h"

#include <stdlib.h>

FILE *srcF;

void _source_file ( FILE *file ) {

    srcF = file;

}


int main (  ) {

    srcF = stdin;

    _source_file( srcF );

    int res = parse(  );

    return res;

}