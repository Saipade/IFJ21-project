/**
 * PROJECT: Implementation of imperative IFJ21 programming language compiler
 * PART: Code generator
 * 
 * AUTHOR(S): Maksim Tikhonov (xtikho00)
 */

#include "errorslist.h"
#include "code_generator.h"
#include "scanner.h"
#include "string_processor.h"
#include "symtable.h"

#include <string.h>

Dynamic_string *codeString;

void _code_string ( Dynamic_string *string ) {

    codeString = string;

}

void print_out (  ) {
    
    printf( "%s", codeString->str );

}

void cg_output ( FILE *outputFile ) {

    fprintf( outputFile, "%s", codeString->str );
    ds_free( codeString );

}

/* .......................................... MAIN FUNCTION FRAME GENERATION .......................................... */


bool cg_start (  ) {
    // prologue
    ADD_LINE("");
    ADD_LINE( ".IFJcode21" );
    // define global variables for some operations
    ADD_LINE( "DEFVAR GF@%tmp0" );
    ADD_LINE( "DEFVAR GF@%tmp1" );
    ADD_LINE( "DEFVAR GF@%tmp2" );
    ADD_LINE( "DEFVAR GF@%expResult" );
    ADD_LINE( "DEFVAR GF@%return" );
    // add built-in functions
    ADD_LINE( FUNCTION_READS );
    ADD_LINE( FUNCTION_READI );
    ADD_LINE( FUNCTION_READN );
    ADD_LINE( FUNCTION_WRITE );
    ADD_LINE( FUNCTION_TOINTEGER );
    ADD_LINE( FUNCTION_SUBSTR );
    ADD_LINE( FUNCTION_ORD );
    ADD_LINE( FUNCTION_CHR );

    return true;

}

bool cg_end (  ) {

    ADD_LINE( "POPFRAME" );
    ADD_LINE( "CLEARS" );

    return true;

}

/* .......................................... FUNCTION FRAME CODE GENERATION .......................................... */

bool cg_function_header ( char *functionId ) {

    ADD_LINE( "" );             // generate jump over function definition
    ADD_CODE( "JUMP $over$" );
    ADD_LINE( functionId );
    ADD_LINE( "" );

    ADD_CODE( "# Function " );
    ADD_CODE( functionId );
    ADD_LINE( ":" );
    ADD_LINE( "" );
    // function start label
    ADD_CODE( "LABEL $" );
    ADD_LINE( functionId );
    ADD_LINE( "CREATEFRAME" );
    ADD_LINE( "PUSHFRAME" );

    return true;

}

bool cg_function_param ( Parser_data *parserData ) {

    char strIndex1[2]; // parameter index
    char strIndex2[2]; // depth
    sprintf( strIndex1, "%d", parserData->paramIndex );
    sprintf( strIndex2, "%d", parserData->currentDepth );
    // declare variable inputId
    ADD_CODE( "DEFVAR LF@%");
    ADD_CODE( strIndex2 );
    ADD_CODE( "%" );
    ADD_LINE( parserData->currentVar->id );

    ADD_CODE( "MOVE LF@%");
    ADD_CODE( strIndex2 );
    ADD_CODE( "%" );
    ADD_CODE( parserData->currentVar->id );
    ADD_CODE( " LF@%");
    ADD_LINE( strIndex1 );

    return true;

}

bool cg_function_retval ( int index ) {
    
    char strIndex[2];
    sprintf( strIndex, "%d", index );
    // declare output variable
    ADD_CODE( "DEFVAR LF@%1%retval");
    ADD_LINE( strIndex );
    // assign type to it
    ADD_CODE( "MOVE LF@%1%retval");
    ADD_CODE( strIndex );
    ADD_LINE( " nil@nil" ); // for correct return 

    return true;

}

bool cg_function_retval_get_value ( int index ) {

    char strIndex[2];
    sprintf( strIndex, "%d", index );
    ADD_CODE( "MOVE LF@%1%retval" );
    ADD_CODE( strIndex );
    ADD_LINE( " GF@expResult");

    return true;

}

bool cg_jump_to_end ( char *functionId ) {

    ADD_CODE( "JUMP $" );
    ADD_CODE( functionId );
    ADD_LINE( "$ret" );

    return true;

}

bool cg_function_return ( char *functionId ) {

    ADD_CODE( "LABEL $" ); 
    ADD_CODE( functionId ); 
    ADD_LINE( "$ret" );
	ADD_LINE( "POPFRAME" );
	ADD_LINE( "RETURN" );
    
    ADD_LINE( "" );
    ADD_CODE( "# End of function " );
    ADD_LINE( functionId );

    ADD_LINE( "" );     // generate label for jump
    ADD_CODE( "LABEL $over$" );
    ADD_LINE( functionId );
    ADD_LINE( "" );

    return true;

}

/*.......................................... FUNCTION CALL ..........................................*/


bool cg_call ( char *functionId ) {

    ADD_CODE( "CALL $" );
    ADD_LINE( functionId );

    return true;

}

bool cg_frame_to_pass_param (  ) {

    ADD_LINE( "CREATEFRAME" );

    return true;

}

bool cg_pass_param ( Parser_data *parserData ) {

    char strIndex[2];
    sprintf( strIndex, "%d", parserData->paramIndex );
    ADD_CODE( "DEFVAR TF@%" );
    ADD_LINE( strIndex );

    ADD_CODE( "MOVE TF@%" );
    ADD_CODE( strIndex );
    ADD_LINE( " GF@%expResult" );

    return true;

}

bool cg_pass_param_light ( Parser_data *parserData ) {

    char strIndex[2];
    sprintf( strIndex, "%d", parserData->paramIndex );
    ADD_CODE( "DEFVAR TF@%" );
    ADD_LINE( strIndex );

    ADD_CODE( "MOVE TF@%" );
    ADD_CODE( strIndex );
    ADD_CODE( " " );
    if (!cg_term( &parserData->token, parserData->currentVar->depth )) return false;

    return true;

}

bool cg_get_retval ( Item_data *variable, int index ) {

    char strIndex1[2]; // variable depth
    char strIndex2[2]; // retval index
    sprintf( strIndex1, "%d", variable->depth );
    sprintf( strIndex2, "%d", index );
    
    ADD_CODE( "MOVE LF@%" );
    ADD_CODE( strIndex1 );
    ADD_CODE( "%" );
    ADD_CODE( variable->id );
    
    ADD_CODE( " TF@%retval" );
    ADD_LINE( strIndex2 );

    return true;

}

bool cg_declare_var ( Parser_data *parserData ) {

    char strDepth[2];
    sprintf( strDepth, "%d", parserData->currentDepth );
    ADD_CODE( "DEFVAR LF@%" );
    ADD_CODE( strDepth );
    ADD_CODE( "%" );
    ADD_LINE( parserData->currentVar->id );  

    return true;

}

bool cg_define_var ( Parser_data *parserData ) {
    
    char strDepth[2];
    sprintf( strDepth, "%d", parserData->currentDepth );
    ADD_CODE( "MOVE LF@%" );
    ADD_CODE( strDepth );
    ADD_CODE( "%" );
    ADD_CODE( parserData->lhsId->id );
    ADD_CODE( " " );
    if (!cg_process_data_type( parserData->lhsId->type )) return false;
    ADD_LINE( "" );

    return true;

}

bool cg_push ( Token *token, int index ) {

    ADD_CODE( "PUSHS " );
    if (!cg_term( token, index )) return false;

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

// form : for vars - fram@%depth%id, for const - type@val
bool cg_term ( Token *token, int index ) {

    Dynamic_string str;
    Dynamic_string *tmpString = &str;
    if (!ds_init( tmpString )) return false;
    
    char code[64];
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
        
            if (!ds_copy( token->attribute.string, tmpString )) exit(ERR_INTERNAL);
            ADD_CODE( "string@" );
            ADD_CODE( tmpString->str );

        break;

        case (T_IDE): 

            sprintf( code, "%d", index);
            ADD_CODE( "LF@%" );
            ADD_CODE( code ); // (depth)
            ADD_CODE( "%" );
            ADD_CODE( token->attribute.string->str );
            
        break;

        /* case (T_BOO): until better times

            ADD_CODE( "bool@false" );

        break; */

        case (T_NIL):

            ADD_CODE( "nil@nil" );

        break;

        case (T_NDA):

        default:

            ds_free( tmpString );
            return false;

        break;


    }

    ADD_LINE( "" );

    ds_free( tmpString );
    return true;

}

bool cg_convert_1st_num2int (  ) {

    ADD_LINE( "FLOAT2INTS" );

    return true;

} 

bool cg_convert_2nd_num2int (  ) {

    ADD_LINE( "POPS GF@%tmp0" );
    ADD_LINE( "FLOAT2INTS" );
    ADD_LINE( "PUSHS GF@%tmp0" );

    return true;
    
}

bool cg_convert_both_num2int (  ) {

    if (!cg_convert_1st_num2int(  )) return false;
    if (!cg_convert_2nd_num2int(  )) return false;

    return true;

}

bool cg_convert_1st_int2num (  ) {

    ADD_LINE( "INT2FLOATS" );

    return true;

}

bool cg_convert_2nd_int2num (  ) {

    ADD_LINE( "POPS GF@%tmp0" );
    ADD_LINE( "INT2FLOATS" );
    ADD_LINE( "PUSHS GF@%tmp0" );

    return true;

}

bool cg_convert_both_int2num (  ) {

    if (!cg_convert_1st_int2num(  )) return false;
    if (!cg_convert_2nd_int2num(  )) return false;

    return true;

}

bool cg_convert_res_num2int (  ) {

    ADD_LINE( "FLOAT2INT GF@expResult GF@expResult" );

    return true;

}

bool cg_convert_res_int2num (  ) {

    ADD_LINE( "INT2FLOAT GF@expResult GF@expResult" );

    return true;

}

/* .......................................... IF/WHILE STATEMENT .......................................... */

bool cg_if_header ( int index, int depth ) {

    char *strIndex;
    char *strDepth;

    sprintf( strIndex, "%d", index );
    sprintf( strDepth, "%d", depth );

    ADD_CODE( "JUMPIFEQ $");
    ADD_CODE( "%" );
    


    return true;
    
}

/* .......................................... EXPRESSION .......................................... */

bool cg_save_result (  ) {

    ADD_LINE( "POPS GF@%expResult");

    return true;

}

bool cg_save_to ( Parser_data *parserData ) {

    char strDepth[2];
    sprintf( strDepth, "%d", parserData->currentDepth );
    ADD_CODE( "MOVE " );
    ADD_CODE( "LF@%" );
    ADD_CODE( strDepth );
    ADD_CODE( "%" );
    ADD_CODE( parserData->lhsId->id );
    ADD_LINE( " GF@%expResult");

    return true;

}

bool cg_lens (  ) {

    ADD_LINE( "POPS GF@%tmp0" );
    ADD_LINE( "STRLEN GF@%tmp1 GF@%tmp0" );
    ADD_LINE( "PUSHS GF@%tmp1" );

    return true;

}

bool cg_lens_both (  ) {

    ADD_LINE( "POPS GF@%tmp0" );
    ADD_LINE( "POPS GF@%tmp1" );
    ADD_LINE( "STRLEN GF@%tmp2 GF@%tmp0" );
    ADD_LINE( "STRLEN GF@%tmp1 GF@%tmp1" );
    ADD_LINE( "PUSHS GF@%tmp2" );
    ADD_LINE( "PUSHS GF@%tmp1" );

    return true;

}

bool cg_adds (  ) {

    ADD_LINE( "ADDS" );

    return true;

}

bool cg_subs (  ) {

    ADD_LINE( "SUBS" );

    return true;

}

bool cg_muls (  ) {

    ADD_LINE( "MULS" );

    return true;
    
}

bool cg_divs (  ) {

    ADD_LINE( "DIVS" );

    return true;
    
}

bool cg_idivs (  ) {

    ADD_LINE( "IDIVS" );

    return true;
    
}

bool cg_cats (  ) {

    ADD_LINE( "POPS GF@%tmp0" );
    ADD_LINE( "POPS GF@%tmp1" );
    ADD_LINE( "CONCAT GF@%tmp1 GF@%tmp1 GF@%tmp0");
    ADD_LINE( "PUSHS GF@%tmp1" );

    return true;
    
}

bool cg_lths (  ) {

    ADD_LINE( "LTS" );

    return true;

}

bool cg_lets (  ) {

    ADD_LINE( "POPS GF@%tmp0" );
    ADD_LINE( "POPS GF@%tmp1" );
    ADD_LINE( "PUSHS GF@%tmp1" );
    ADD_LINE( "PUSHS GF@%tmp0" );
    ADD_LINE( "PUSHS GF@%tmp1" );
    ADD_LINE( "PUSHS GF@%tmp0" );

    ADD_LINE( "LTS" );
    ADD_LINE( "POPS GF@%tmp0" );
    ADD_LINE( "EQS" );
    ADD_LINE( "PUSH GF@%tmp0" );

    ADD_LINE( "ORS" );

    return true;

}

bool cg_mths (  ) {

    ADD_LINE( "GTS" );

    return true;

}

bool cg_mets (  ) {

    ADD_LINE( "POPS GF@%tmp0" );
    ADD_LINE( "POPS GF@%tmp1" );
    ADD_LINE( "PUSHS GF@%tmp1" );
    ADD_LINE( "PUSHS GF@%tmp0" );
    ADD_LINE( "PUSHS GF@%tmp1" );
    ADD_LINE( "PUSHS GF@%tmp0" );

    ADD_LINE( "GTS" );
    ADD_LINE( "POPS GF@%tmp0" );
    ADD_LINE( "EQS" );
    ADD_LINE( "PUSH GF@%tmp0" );

    ADD_LINE( "ORS" );

    return true;

}

bool cg_equs (  ) {

    ADD_LINE( "EQS" );

    return true;

}

bool cg_neqs (  ) {

    ADD_LINE( "EQS" );
    ADD_LINE( "NOTS" );

    return true;

}


