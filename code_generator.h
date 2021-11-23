/**
 * Header file of code generator
 */

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "expression.h"


// Append string to result code
#define ADD_CODE(CODE)														\
	if (!ds_add_chars( codeString, CODE )) return false


// Append string with \n to result code
#define ADD_LINE(CODE)														\
	if (!ds_add_chars( codeString, CODE )) return false;					\
	if (!ds_add_chars( codeString, "\n" )) return false;


// Definition of built-in functions

// function reads() : string
#define FUNCTION_READS                                                      \
    "# Reads function\n"                                                    \
    "LABEL $reads\n"                                                        \
    "PUSHFRAME\n"                                                           \
    "DEFVAR LF@%retval\n"                                                   \
    "READ LF@%retval string\n"                                              \
                                                                            \
    "LABEL $reads$ret\n"                                                    \
    "POPFRAME\n"                                                            \
    "RETURN\n"                                                 

// function readi() : integer
#define FUNCTION_READI                                                      \
    "# Readi function\n"                                                    \
    "LABEL $readi\n"                                                        \
    "PUSHFRAME\n"                                                           \
    "DEFVAR LF@%retval\n"                                                   \
    "READ LF@%retval int\n"                                                 \
                                                                            \
    "LABEL $readi$ret\n"                                                    \
    "POPFRAME\n"                                                            \
    "RETURN\n"
    
// function readn() : number
#define FUNCTION_READN                                                      \
    "# Readn function\n"                                                    \
    "LABEL $readn\n"                                                        \
    "PUSHFRAME\n"                                                           \
    "DEFVAR LF@%retval\n"                                                   \
    "READ LF@%retval float\n"                                               \
                                                                            \
    "LABEL $readn$ret\n"                                                    \
    "POPFRAME\n"                                                            \
    "RETURN\n"
    
// function write
#define FUNCTION_WRITE                                                      \
    "# Write function\n"                                                    \
    "LABEL $write\n"                                                        \
    "PUSHFRAME\n"                                                           \
    "WRITE LF@%0\n"                                                         \
                                                                            \
    "LABEL $write$ret\n"                                                    \
    "POPFRAME\n"                                                            \
    "RETURN\n"

// function tointeger(f : number) : integer
#define FUNCTION_TOINTEGER                                                  \
    "# Tointeger function\n"                                                \
    "LABEL $tointeger\n"                                                    \
    "PUSHFRAME\n"                                                           \
    "DEFVAR LF@%retval\n"                                                   \
    "DEFVAR LF@%nilcheck\n"                                                 \
    "EQ LF@%nilcheck LF@%0 nil@nil\n"                                       \
    "JUMPIFEQ $tointeger$nil LF@nilcheck bool@true\n"                       \
                                                                            \
    "FLOAT2INT LF@%retval LF@%0\n"                                          \
    "JUMP $tointeger$ret\n"                                                 \
                                                                            \
    "LABEL $tointeger$nil\n"                                                \
    "MOVE LF@%retval nil@nil\n"                                             \
    "LABEL $tointeger$ret\n"                                                \
    "POPFRAME\n"                                                            \
    "RETURN\n"

// function substr(s : string, i : number, j : number) : string
#define FUNCTION_SUBSTR                                                     \
    "# SUBSTR function\n"                                                   \
    "LABEL $substr\n"                                                       \
    "PUSHFRAME"                                                             \
    "DEFVAR LF@%s\n"                                                        \
    "MOVE LF@%s LF@%0\n"                                                    \
    "DEFVAR LF@%i\n"                                                        \
    "MOVE LF@%i LF@%1\n"                                                    \
    "DEFVAR LF@%j\n"                                                        \
    "MOVE LF@%j LF@%2\n"                                                    \
    "DEFVAR LF@%retval0\n"                                                  \
    "MOVE LF@%retval0 nil@nil\n"                                            \
    "DEFVAR LF@%check\n"                                                    \
    "MOVE LF@%check bool@false\n"                                           \
                                                                            \
    "GT LF@%check LF@%i LF@%j\n"                                            \
    "JUMPIFEQ $substr$emptystr LF@%check bool@true\n"                       \
                                                                            \
    "EQ LF@%check LF@%s nil@nil\n"                                          \
    "JUMPIFEQ $substr$err8 LF@%check bool@true\n"                           \
    "EQ LF@%check LF@%i nil@nil\n"                                          \
    "JUMPIFEQ $substr$err8 LF@%check bool@true\n"                           \
    "EQ LF@%check LF@%j nil@nil\n"                                          \
    "JUMPIFEQ $substr$err8 LF@%check bool@true\n"                           \
                                                                            \
    "DEFVAR LF@%strlen\n"                                                   \
    "STRLEN LF@%strlen LF@%s\n"                                             \
                                                                            \
    "GT LF@%check LF@%i LF@%strlen\n"                                       \
    "JUMPIFEQ $substr$emptystr LF@%check bool@true\n"                       \
    "GT LF@%check int@1 LF@%i\n"                                            \
    "JUMPIFEQ $substr$emptystr LF@%check bool@true\n"                       \
                                                                            \
    "GT LF@%check LF@%j LF@%strlen\n"                                       \
    "JUMPIFEQ $substr$emptystr LF@%check bool@true\n"                       \
    "GT LF@%check int@1 LF@%j\n"                                            \
    "JUMPIFEQ $substr$emptystr LF@%check bool@true\n"                       \
                                                                            \
    "DEFVAR LF@%char\n"                                                     \
    "SUB LF@%i LF@%i int@1\n"                                               \
    "SUB LF@%j LF@%j int@1\n"                                               \
    "LABEL $substr$loop\n"                                                  \
    "GETCHAR LF@%char LF@%s LF@%i LF@%i\n"                                  \
    "CONCAT LF@%retval0 LF@%retval0 LF@%char\n"                             \
    "ADD LF@%i LF@%i int@1\n"                                               \
    "GT LF@%check LF@%i LF@%j\n"                                            \
    "JUMPIFEQ $substr$ret LF@%check bool@true\n"                            \
                                                                            \
    "LABEL $substr$emptystr\n"                                              \
    "MOVE LF@%retval0 string@ \n"                                           \
    "JUMP $substr$ret\n"                                                    \
                                                                            \
    "LABEL $substr$err8\n"                                                  \
    "MOVE LF@%retval0 int@8\n"                                              \
    "JUMP $substr$ret\n"                                                    \
                                                                            \
    "LABEL $substr$ret\n"                                                   \
    "POPFRAME\n"                                                            \
    "RETURN\n"                                                              \

// function ord(s : string, i : integer) : integer 
#define FUNCTION_ORD                                                        \
    "# Ord function\n"                                                      \
    "LABEL $ord\n"                                                          \
    "PUSHFRAME\n"                                                           \
    "DEFVAR LF@%s\n"                                                        \
    "MOVE LF@%s LF@%0\n"                                                    \
    "DEFVAR LF@%i\n"                                                        \
    "MOVE LF@%i LF@%1\n"                                                    \
    "DEFVAR LF@%retval0\n"                                                  \
    "MOVE LF@%retval0 nil@nil\n"                                            \
    "DEFVAR LF@%check\n"                                                    \
    "MOVE LF@%check bool@false\n"                                           \
                                                                            \
    "EQ LF@%check LF@%s nil@nil\n"                                          \
    "JUMPIFEQ $ord$err8 LF@%check bool@true\n"                              \
    "EQ LF@%check LF@%i nil@nil\n"                                          \
    "JUMPIFEQ $ord$err8 LF@%check bool@true\n"                              \
                                                                            \
    "DEFVAR LF@%strlen\n"                                                   \
    "STRLEN LF@%strlen LF@%s\n"                                             \
                                                                            \
    "GT LF@%check LF@%i LF@%strlen\n"                                       \
    "JUMPIFEQ $substr$emptystr LF@%check bool@true\n"                       \
    "GT LF@%check int@1 LF@%i\n"                                            \
    "JUMPIFEQ $substr$emptystr LF@%check bool@true\n"                       \
                                                                            \
    "SUB LF@%i LF@%i int@1\n"                                               \
    "STRI2INT LF@%retval0 LF@%s LF@%i\n"                                    \
    "JUMP $ord$ret\n"                                                       \
                                                                            \
    "LABEL $ord$emptystr\n"                                                 \
    "MOVE LF@%retval0 nil@nil\n"                                            \
    "JUMP $ord$ret\n"                                                       \
                                                                            \
    "LABEL $ord$err8\n"                                                     \
    "MOVE LF@%retval0 int@8\n"                                              \
    "JUMP $ord$ret\n"                                                       \
                                                                            \
    "LABEL $ord$ret\n"                                                      \
    "POPFRAME\n"                                                            \
    "RETURN\n"                                                              \
    
// function chr(i : integer) : string
#define FUNCTION_CHR                                                        \
    "# Chr function\n"                                                      \
    "LABEL $chr\n"                                                          \
    "PUSHFRAME\n"                                                           \
    "DEFVAR LF@%i\n"                                                        \
    "MOVE LF@%i LF@%0\n"                                                    \
    "DEFVAR LF@%retval0\n"                                                  \
    "MOVE LF@%retval0 nil@nil\n"                                            \
    "DEFVAR LF@%check\n"                                                    \
    "MOVE LF@%check bool@false\n"                                           \
                                                                            \
    "EQ LF@%check LF@%i nil@nil\n"                                          \
    "JUMPIFEQ $chrr$err8 LF@%check nil@nil\n"                               \
                                                                            \
    "GT LF@%check LF@%i int@255\n"                                          \
    "JUMPIFEQ $chr$ret LF@%check bool@true\n"                               \
    "GT LF@%check int@0 LF@%i\n"                                            \
    "JUMPIFEQ $chr$ret LF@%check bool@true\n"                               \
                                                                            \
    "INT2CHAR LF@%retval0 LF@%i\n"                                          \
    "JUMP $ord$ret\n"                                                       \
                                                                            \
    "LABEL $chr$err8\n"                                                     \
    "MOVE LF@%retval0 int@8\n"                                              \
    "JUMP $chr$ret\n"                                                       \
                                                                            \
    "LABEL $chr$ret\n"                                                      \
    "POPFRAME\n"                                                            \
    "RETURN\n"

// prints entire result code (for debugging reasons. to delete before commit)
void print_out (  );

/**
 * @brief Prints out result code to destination file (STDOUT)
 * @param outoutFile file the result code will be written to
 */
void cg_output ( FILE *outputFile );

/**
 * @brief Sets pointer to the string initialized in parser
 * @param string given string
 */
void _code_string ( Dynamic_string *string );

/**
 * @brief Generates code for the start of the program (built-in functions, jump to main)
 */
bool cg_start (  );

/**
 * @brief Generates code for the end of program (stack clear, popframe)
 */
bool cg_end (  );

/**
 * @brief Generates code for header of function
 * @param functionId function identifier
 */
bool cg_function_header ( char *functionId );

/**
 * @brief Generates code for function input variables
 * @param inputId identifier of given input
 * @param dataType data type of given input
 * @param index index of given input
 */
bool cg_function_input_type ( char *inputId, Data_type dataType, int index );

/**
 * @brief Generates code for function output
 * @param dataType data type of given parameter
 * @param index index of given parameter
 */
bool cg_function_output_type ( Data_type dataType, int index );

/**
 * @brief Generates code for function return (LABEL, RET)
 * @param functionId Function identifier
 */
bool cg_function_return ( char *functionId );

/**
 * @brief Generates code for variable declaration
 * @param variableId variable identifier
 */
bool cg_var_decl ( char *variableId );

/**
 * @brief Generates code for function call
 * @param functionId called function name
 */
bool cg_call ( char *functionId );

/**
 * @brief Generates frame to pass params to function
 */
bool cg_frame_to_pass_param (  );

/**
 * @brief Generates code to pass params to function
 * @param token given token 
 * @param index index of param to be passed
 */
bool cg_pass_param ( Token *token, int index );

/**
 * @brief Generates value 
 * @param token contains required information
 */
bool cg_term ( Token *token );

/**
 * @brief Generates code for variable declaration
 * @param variableId variable identifier
 */
bool cg_declare_var ( char *variableId );

/**
 * @brief Generates code for variable defenition
 * @param item contains required data 
 */
bool cg_define_var ( Item_data *item );

/**
 * @brief Generates push
 * @param token 
 */
bool cg_push ( Token *token );

/**
 * @brief Auxiliary function. Generates type of variable
 * @param dataType given data type
 */
bool cg_process_data_type ( Data_type dataType );

/**
 * @brief Converts value on the top of the stack from number to integer
 */
bool cg_convert_1st_num2int (  );

/**
 * @brief Converts value on the 2nd position of the stack from number to integer
 */
bool cg_convert_2nd_num2int (  );

/**
 * @brief Converts both operands to integer
 */
bool cg_convert_both_num2int (  );

/**
 * @brief Converts value on the top of the stack from integer to number
 */
bool cg_convert_1st_int2num (  );

/**
 * @brief Converts value on the 2nd position of the stack from integer to number
 */
bool cg_convert_2nd_int2num (  );

/**
 * @brief Converts both operands to number
 */
bool cg_convert_both_int2num (  );

/**
 * @brief 
 */
bool cg_if_header ( int index, int depth );

/**
 * @brief Generates code for stack pop and pass value to variable
 * @param variableId variable identifier to which the value will be passed
 */
bool cg_pop_to ( char *variableId );

/**
 * @brief Generates code for stack version if STRLEN operation
 */
bool cg_lens(  );

/**
 * @brief Generates code for ADDS
 */
bool cg_adds (  );

/**
 * @brief Generates code for SUBS
 */
bool cg_subs (  );

/**
 * @brief Generates code for MULS
 */
bool cg_muls (  );

/**
 * @brief Generates code for DIVS
 */
bool cg_divs (  );

/**
 * @brief Generates code for IDIVS
 */
bool cg_idivs (  );

/**
 * @brief Generates code for stack version of CONCAT
 */
bool cg_cats (  );

/**
 * @brief Generates code for LTS
 */
bool cg_lths (  );

/**
 * @brief Generates code for stack version of "less than or equal to"
 */
bool cg_lets (  );

/**
 * @brief Generates code for GTS
 */
bool cg_mths (  );

/**
 * @brief Generates code for stack version of "more than or equal to"
 */
bool cg_mets (  );

/**
 * @brief Generates code for EQS
 */
bool cg_equs (  );

/**
 * @brief Generates code for stack version of "not" EQ
 */
bool cg_neqs (  );

#endif