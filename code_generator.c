/*

*/

#include "code_generator.h"
#include "scanner.h"
#include "string_processor.h"
#include "symtable.h"
#include "parser.h"

#include <string.h>

Dynamic_string *codeString;

void _code_string ( Dynamic_string *string ) {

    codeString = string;

}

// Append string to result code
#define ADD_CODE(CODE)														\
	if (!ds_add_chars( codeString, CODE )) return false


// Append string with \n to result code
#define ADD_LINE(CODE)														\
	if (!ds_add_chars( codeString, CODE )) return false;					\
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


bool cg_function_header ( char *functionId ) {

    // commentary on the start of function
    ADD_CODE( "\n# Function " );
    ADD_CODE( functionId );
    ADD_LINE( ":" );
    ADD_LINE( "" );
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

    ADD_CODE( "LABEL $" ); 
    ADD_CODE( functionId ); 
    ADD_CODE( "$return\n" );
	ADD_LINE( "POPFRAME" );
	ADD_LINE( "RETURN" );
    
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

bool cg_frame_to_pass_param (  ) {

    ADD_LINE( "CREATEFRAME" );

    return true;

} 

bool cg_pass_param ( Token *token, int index ) {

    char strIndex[2];
    sprintf( strIndex, "%d", index );
    ADD_CODE( "DEFVAR TF@%" );
    ADD_CODE( strIndex );
    if (!cg_term( token )) return false;
    ADD_LINE( "" );

}

bool cg_term ( Token *token ) {

    Dynamic_string str;
    Dynamic_string *tmpString = &str;
    if (!ds_init( tmpString )) return false;

    char code[32];
    char c;

    switch (token->type) {

        case (T_INT):

            sprintf( code, "%d", token->attribute.integer );
            ADD_CODE( "int@" );
            ADD_CODE( code );

        break;

        case (T_NUM):

            sprintf( code, "%a", token->attribute.floating );
            ADD_CODE( "float@" );
            ADD_CODE( code );

        break;

        case (T_STR):

            for (int i = 0; c = (char) token->attribute.string->str[i] != '\0'; i++) {

                if (c == '\\' || c == '#' || c <= 32 ) {
                    ds_add_char( tmpString, '\\' );
                    sprintf( code, "%03d", c );
                    ds_add_chars( tmpString, code );
                } 
                
                else {
                    ds_add_char( tmpString, c );
                }

            }

            ADD_CODE( "string@" );
            ADD_CODE( tmpString->str );

        break;

        

        case (T_IDE): 

            ADD_CODE( "GF@" );
            ADD_CODE( tmpString->str );
            
        break;

        // case (T_BOO): until better times

            // ADD_CODE( "bool@false" );

        // break;

        case (T_NIL):

            ADD_CODE( "nil@nil" );

        break;

        case (T_NDA):

        default:

            ds_free( tmpString );
            return false;

        break;


    }

    ds_free( tmpString );
    return true;

}

bool cg_declare_var ( Item_data *item ) {

    ADD_CODE( "DEFVAR LF@" );
    ADD_LINE( item->id );

    return true;

}

bool cg_define_var ( Item_data *item ) {
    
    ADD_CODE( "MOVE LF@ " );
    ADD_CODE( item->id );
    ADD_CODE( " " );
    if (!cg_process_data_type( item->type )) return false;
    ADD_LINE( "" );

    return true;

}

bool cg_push ( Token *token ) {

    ADD_CODE( "PUSHS " );
    if (!cg_term( token )) return false;
    ADD_LINE( "" );

    return true;

}

/* .......................................... AUXILLIARY FUNCTIONS .......................................... */

bool cg_process_data_type ( Data_type dataType ) {

    switch (dataType) {

        case (T_INT):

            ADD_CODE( "int@0");

        break;

        case (T_NUM): 

            ADD_CODE( "float@0.0");

        break;

        case (T_STR):

            ADD_CODE( "string@" );

        break;

        //case (T_BOO):

        //    ADD_CODE( "bool@false");

        //break;

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

bool cg_operation ( Data_type type, pt_rule rule ) {

    switch (rule) {

        case LEN_E:

            ADD_CODE( "STRLEN " );

        break;

        case E_PLUS_E:

            ADD_CODE( "ADDS " );

        break;

        case E_MINUS_E:

            ADD_CODE( "SUBS " );

        break;

        case E_MUL_E:

            ADD_CODE( "MULS " );

        break;

        

    }



}