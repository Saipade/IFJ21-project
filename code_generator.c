/*

*/

#include "code_generator.h"
#include "scanner.h"
#include "string_processor.h"
#include "symtable.h"

#include <string.h>

Dynamic_string * codeString;

#define ADD_CODE(CODE)                                                      \
    if (!ds_add_chars( codeString, CODE )) return false


// Append string with \n to result code
#define ADD_LINE(CODE)                                                      \
    if (!ds_add_chars( codeString, CODE )) return false;                    \
    if (!ds_add_chars( codeString, "\n" )) return false;



bool cg_start (  ) {

    ADD_LINE(".IFJcode21");

    ADD_LINE( FUNCTION_READS );
    ADD_LINE( FUNCTION_READI );
    ADD_LINE( FUNCTION_READS );
    ADD_LINE( FUNCTION_WRITE );
    ADD_LINE( FUNCTION_TOINTEGER );
    ADD_LINE( FUNCTION_SUBSTR );
    ADD_LINE( FUNCTION_ORD );
    ADD_LINE( FUNCTION_CHR );

    return true;

}

/* .......................................... FUNCTION FRAME CODE GENERATION .......................................... */

/* function header of form  
    LABEL $functionId
    PUSHFRAME        
*/
bool cg_function_header ( char *functionId ) {

    // commentary on the start of function
    ADD_CODE( "\n# Function" );
    ADD_CODE( functionId );
    ADD_LINE( ":" );
    // function start label
    ADD_CODE( "LABEL $" );
    ADD_LINE( functionId );
    ADD_LINE( "PUSHFRAME" );

    return true;

}

bool cg_function_input_type ( char *inputId, Data_type dataType, int index ) {

    char strIndex[32];
    sprintf( strIndex, "%d", index );
    // declare variable inputId
    ADD_CODE( "DEFVAR LF@");
    ADD_LINE( inputId );
    // pass index' output value to it
    ADD_CODE( "MOVE LF@" );
    ADD_CODE( inputId );
    ADD_CODE( " LF@%" );
    ADD_LINE( strIndex );

    return true;

}


bool cg_function_output_type ( Data_type dataType, int index ) {
    
    char strIndex[32];
    sprintf( strIndex, "%d", index );
    // declare output variable
    ADD_CODE( "DEFVAR LF@%output");
    ADD_LINE( strIndex );
    // assign type to it
    ADD_CODE( "MOVE LF@%output");
    ADD_CODE( strIndex );
    ADD_CODE( " " );
    if (!cg_process_data_type( dataType )) return false;
    ADD_LINE( "" );

    return true;

}

bool cg_function_return ( char *functionId ) {

    // function return label
    ADD_CODE( "LABEL $" ); 
    ADD_CODE( functionId ); 
    ADD_CODE( "$return\n" );
	ADD_LINE( "POPFRAME" );
	ADD_LINE( "RETURN" );
    // commentary on the end of function
    ADD_CODE( "\n# End of function" );
    ADD_LINE( functionId );

    return true;

}

/* ... */

bool cg_var_decl ( char *variableId ) {

    ADD_CODE( "DEFVAR LF@");
    ADD_LINE( variableId );  

    return true;  

}



bool cg_call ( char *functionId ) {

    ADD_CODE( "CALL $" );
    ADD_LINE( functionId );

    return true;

}

/* .......................................... AUXILLIARY FUNCTIONS .......................................... */

bool cg_process_data_type ( Data_type dataType ) {

    switch (dataType) {

        case (T_INT):

            ADD_CODE( "int@0");

        break;

        case (T_DOU): 

            ADD_CODE( "float@0.0");

        break;

        case (T_STR):

            ADD_CODE( "string@" );

        break;

        case (T_BOO):

            ADD_CODE( "bool@false");

        break;

        case (T_NIL):

            ADD_CODE( "nil@nil" );

        break;

        default: 

            return false;

        break;


    }

    return true;

}


/* .......................................... IF/WHILE STATEMENT .......................................... */

bool cg_if_header ( char *functionId ) {

    ADD_CODE( "JUMPIFEQ $");
    

    return true;
    
}