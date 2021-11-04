/*

*/

#include "code_generator.h"
#include "scanner.h"
#include "string_processor.h"

Dynamic_string *resultString;

// Append string to result code
#define ADD_CODE(_code)														\
	if (!ds_add_str(resultString, (_code))) return false


// Append string with \n to result code
#define ADD_LINE(_code)														\
	if (!ds_add_str(resultString, (_code))) return false;					\
	if (!ds_add_str(resultString, "\n")) return false;

void cg_start (  ) {

    ADD_LINE(".IFJcode21");

    ADD_LINE(FUNCTION_READS);
    ADD_LINE(FUNCTION_READI);
    ADD_LINE(FUNCTION_READS);
    ADD_LINE(FUNCTION_WRITE);
    ADD_LINE(FUNCTION_TOINTEGER);
    ADD_LINE(FUNCTION_SUBSTR);
    ADD_LINE(FUNCTION_ORD);
    ADD_LINE(FUNCTION_CHR);

}

void cg_function_label (char *functionId) {

    // commentary
    ADD_CODE( "\n# Function" );
    ADD_CODE( functionId );
    ADD_LINE( ":" );
    // function label
    ADD_CODE( "LABEL $" );
    ADD_LINE( functionId );
    ADD_LINE( "PUSHFRAME" );

}

