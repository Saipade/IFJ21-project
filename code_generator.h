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
    "JUMP $over$reads\n"                                                    \
    "\n"                                                                    \
    "# Function reads\n"                                                    \
    "LABEL $reads\n"                                                        \
    "PUSHFRAME\n"                                                           \
    "DEFVAR LF@%retval0\n"                                                  \
    "READ LF@%retval0 string\n"                                             \
                                                                            \
    "LABEL $reads$ret\n"                                                    \
    "POPFRAME\n"                                                            \
    "RETURN\n"                                                              \
    "\n"                                                                    \
    "LABEL $over$reads\n"

// function readi() : integer
#define FUNCTION_READI                                                      \
    "JUMP $over$readi\n"                                                    \
    "\n"                                                                    \
    "# Function readi\n"                                                    \
    "LABEL $readi\n"                                                        \
    "PUSHFRAME\n"                                                           \
    "DEFVAR LF@%retval0\n"                                                  \
    "READ LF@%retval0 int\n"                                                \
                                                                            \
    "LABEL $readi$ret\n"                                                    \
    "POPFRAME\n"                                                            \
    "RETURN\n"                                                              \
    "\n"                                                                    \
    "LABEL $over$readi\n"
    
// function readn() : number
#define FUNCTION_READN                                                      \
    "JUMP $over$readn\n"                                                    \
    "\n"                                                                    \
    "# Function readn\n"                                                    \
    "LABEL $readn\n"                                                        \
    "PUSHFRAME\n"                                                           \
    "DEFVAR LF@%retval0\n"                                                  \
    "READ LF@%retval0 float\n"                                              \
                                                                            \
    "LABEL $readn$ret\n"                                                    \
    "POPFRAME\n"                                                            \
    "RETURN\n"                                                              \
    "\n"                                                                    \
    "LABEL $over$readn\n"
    
// function write
#define FUNCTION_WRITE                                                      \
    "JUMP $over$write\n"                                                    \
    "\n"                                                                    \
    "# Function write\n"                                                    \
    "LABEL $write\n"                                                        \
    "PUSHFRAME\n"                                                           \
    "WRITE LF@%0\n"                                                         \
                                                                            \
    "LABEL $write$ret\n"                                                    \
    "POPFRAME\n"                                                            \
    "RETURN\n"                                                              \
    "\n"                                                                    \
    "LABEL $over$write\n"

// function tointeger(f : number) : integer
#define FUNCTION_TOINTEGER                                                  \
    "JUMP $over$tointeger\n"                                                \
    "\n"                                                                    \
    "# Function tointeger\n"                                                \
    "LABEL $tointeger\n"                                                    \
    "PUSHFRAME\n"                                                           \
    "DEFVAR LF@%retval0\n"                                                  \
    "DEFVAR LF@%nilcheck\n"                                                 \
    "EQ LF@%nilcheck LF@%0 nil@nil\n"                                       \
    "JUMPIFEQ $tointeger$nil LF@nilcheck bool@true\n"                       \
                                                                            \
    "FLOAT2INT LF@%retval0 LF@%0\n"                                         \
    "JUMP $tointeger$ret\n"                                                 \
                                                                            \
    "LABEL $tointeger$nil\n"                                                \
    "MOVE LF@%retval0 nil@nil\n"                                            \
    "LABEL $tointeger$ret\n"                                                \
    "POPFRAME\n"                                                            \
    "RETURN\n"                                                              \
    "\n"                                                                    \
    "LABEL $over$tointeger\n"

// function substr(s : string, i : number, j : number) : string
#define FUNCTION_SUBSTR                                                     \
    "JUMP $over$substr\n"                                                   \
    "\n"                                                                    \
    "# Function substr\n"                                                   \
    "LABEL $substr\n"                                                       \
    "PUSHFRAME\n"                                                           \
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
    "GETCHAR LF@%char LF@%s LF@%i\n"                                        \
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
    "EXIT int@8\n"                                                          \
                                                                            \
    "LABEL $substr$ret\n"                                                   \
    "POPFRAME\n"                                                            \
    "RETURN\n"                                                              \
    "\n"                                                                    \
    "LABEL $over$substr\n"

// function ord(s : string, i : integer) : integer 
#define FUNCTION_ORD                                                        \
    "JUMP $over$ord\n"                                                      \
    "\n"                                                                    \
    "# Function ord\n"                                                      \
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
    "EXIT int@8\n"                                                          \
                                                                            \
    "LABEL $ord$ret\n"                                                      \
    "POPFRAME\n"                                                            \
    "RETURN\n"                                                              \
    "\n"                                                                    \
    "LABEL $over$ord\n"
    
// function chr(i : integer) : string
#define FUNCTION_CHR                                                        \
    "JUMP $over$chr\n"                                                      \
    "\n"                                                                    \
    "# Function chr\n"                                                      \
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
    "EXIT int@8\n"                                                          \
                                                                            \
    "LABEL $chr$ret\n"                                                      \
    "POPFRAME\n"                                                            \
    "RETURN\n"                                                              \
    "\n"                                                                    \
    "LABEL $over$chr\n"

// modes for convertion dopisat
typedef enum {
    m_exp,
    m_ret,
} Res_mode;
// prints entire result code (for debugging reasons. to delete before commit)
void print_out (  );

/**
 * @brief Prints out result code to destination file (STDOUT)
 * @param outoutFile file the result code will be written to
 * @return Error
 */
void cg_output ( FILE *outputFile );

/**
 * @brief Sets pointer to the string initialized in parser
 * @param string given string
 * @return Error
 */
void _code_string ( Dynamic_string *string );

/**
 * @brief Generates code for the start of the program (built-in functions, jump to main)
 * @return Error
 */
bool cg_start (  );

/**
 * @brief Generates code for the end of program (stack clear, popframe)
 * @return Error
 */
bool cg_end (  );

/**
 * @brief Generates code for header of function
 * @param functionId function identifier
 * @return Error
 */
bool cg_function_header ( char *functionId );

/**
 * @brief Generates code for function input variables
 * @param parserData contains required information
 * @return Error
 */
bool cg_function_param ( Parser_data *parserData );

/**
 * @brief Defines function output
 * @param index index of retval
 * @return Error
 */
bool cg_function_retval ( int index );

/**
 * @brief Sets return values according to expressions after RETURN keyword
 * @param index index of retval
 * @return Error
 */
bool cg_function_retval_get_value ( int index );

/**
 * @brief Generates code for jump to function end
 * @param functionId function name
 * @return Error
 */
bool cg_jump_to_end ( char *functionId );

/**
 * @brief Generates code for function return
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
 * @brief Generates code to pass params to function (works for expr) used only for write func
 * @param parserData contains required information
 * @return error
 */
bool cg_pass_param ( Parser_data *parserData );

/**
 * @brief light version of cg_pass_param (works only for terms)
 * @param parserData contains required information
 * @return error
 */
bool cg_pass_param_light ( Parser_data *parserData );

/**
 * @brief Assigns value after function call
 * @param variable contains variable information
 * @param index current return value index
 * @return error
 */
bool cg_get_retval ( Item_data *variable, int index );

/**
 * @brief Generates value 
 * @param token contains required information
 */
bool cg_term ( Token *token, int index );

/**
 * @brief Generates code for variable declaration
 * @param parserData contains required information
 */
bool cg_declare_var ( Parser_data *parserData );

/**
 * @brief Generates code for variable defenition
 * @param parserData contains required information
 */
bool cg_define_var ( Parser_data *parserData );

/**
 * @brief Generates push
 * @param token 
 */
bool cg_push ( Token *token, int index );

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
 * @brief Converts expression result to integer
 */
bool cg_convert_res_num2int ( Res_mode mode );

/**
 * @brief Converts expression result to number
 */
bool cg_convert_res_int2num ( Res_mode mode );

/**
 * @brief Generates code for conditional jump to else LABEL
 * @param index current "if index"
 * @param functionId function we are currently in name
 * @return Error
 */
bool cg_if_header ( int index, char *functionId );

/**
 * @brief Generates code for else LABEL
 * @param index current "if index"
 * @param functionId function we are currently in name
 * @return Error
 */
bool cg_if_else ( int index, char *functionId );

/**
 * @brief Generates code for end LABEL
 * @param index current "if index"
 * @param functionId function we are currently in
 * @return Error
 */
bool cg_if_end ( int index, char *functionId );


/**
 * 
 */
bool cg_while_header ( int index, char *functionId );

/**
 * 
 */
bool cg_while_end ( int index, char *functionId );

/**
 * 
 */
bool cg_save_result (  );

/**
 * @brief Generates code for stack pop and pass value to variable
 * @param parserData contains required information
 */
bool cg_save_to ( Parser_data *parserData );

/**
 * @brief Generates code for stack version of STRLEN operation
 */
bool cg_lens(  );

/**
 * @brief Generates code for LENS for both stack top operands
 */
bool cg_lens_both (  );

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