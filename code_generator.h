/*

*/

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "expression.h"

// Definition of built-in functions

// function reads() : string
#define FUNCTION_READS                                                  \
    "\n # Reads function"                                               \
    "\n LABEL $reads"                                                   \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%retval"                                              \
    "\n READ LF@%retval string"                                         \
                                                                        \
    "\n LABEL $reads$ret"                                               \
    "\n POPFRAME"                                                       \
    "\n RETURN"                                                 

// function readi() : integer
#define FUNCTION_READI                                                  \
    "\n # Readi function"                                               \
    "\n LABEL $readi"                                                   \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%retval"                                              \
    "\n READ LF@%retval int"                                            \
                                                                        \
    "\n LABEL $readi$ret"                                               \
    "\n POPFRAME"                                                       \
    "\n RETURN"
    
// function readn() : number
#define FUNCTION_READN                                                  \
    "\n # Readn function"                                               \
    "\n LABEL $readn"                                                   \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%retval"                                              \
    "\n READ LF@%retval float"                                          \
                                                                        \
    "\n LABEL $readn$ret"                                               \
    "\n POPFRAME"                                                       \
    "\n RETURN"
    
// function write
#define FUNCTION_WRITE                                                  \
    "\n # Write function"                                               \
    "\n LABEL $write"                                                   \
    "\n PUSHFRAME"                                                      \
    "\n WRITE LF@%1"                                                    \
    "\n MOVE GF@$res nil@nil"                                           \
                                                                        \
    "\n LABEL $write$ret"                                               \
    "\n POPFRAME"                                                       \
    "\n RETURN"

// function tointeger(f : number) : integer
#define FUNCTION_TOINTEGER                                              \
    "\n # Tointeger function"                                           \
    "\n LABEL $tointeger"                                               \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%retval"                                              \
    "\n FLOAT2INT LF@%retval LF@%1"                                     \
                                                                        \
    "\n LABEL $tointeger$ret"                                           \
    "\n POPFRAME"                                                       \
    "\n RETURN"

// function substr(s : string, i : number, j : number) : string
#define FUNCTION_SUBSTR                                                 \
    "\n # Substr function"                                              \
    "\n LABEL $substr"                                                  \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%retval"                                              \
    "\n DEFVAR LF@tmpChar"                                              \
    "\n DEFVAR LF@typeCheck"                                            \
    "\n DEFVAR LF@lenCheck"                                             \
    "\n DEFVAR LF@strLen"                                               \
    "\n DEFVAR LF@index"                                                \
    "\n MOVE LF@%retval string@"                                        \
    "\n STRLEN LF@strLen LF@%1"                                         \
                                                                        \
    "\n EQ LF@typeCheck LF@%1 nil@nil"                                  \
    "\n JUMPIFEQ $substr$errRet LF@typeCheck bool@true"                 \
    "\n EQ LF@typeCheck LF@%2 nil@nil"                                  \
    "\n JUMPIFEQ $substr$errRet LF@typeCheck bool@true"                 \
    "\n EQ LF@typeCheck LF@%3 nil@nil"                                  \
    "\n JUMPIFEQ $substr$errRet LF@typeCheck bool@true"                 \
                                                                        \
    "\n GT LF@lenCheck int@1 LF@%2"                                     \
    "\n JUMPIFEQ $substr$ret LF@lenCheck bool@true"                     \
    "\n GT LF@lenCheck LF@%2 LF@strLen"                                 \
    "\n JUMPIFEQ $substr$ret LF@lenCheck bool@true"                     \
    "\n GT LF@lenCheck int@1 LF@%3"                                     \
    "\n JUMPIFEQ $substr$ret LF@lenCheck bool@true"                     \
    "\n GT LF@lenCheck LF@%3 LF@strLen"                                 \
    "\n JUMPIFEQ $substr$ret LF@lenCheck bool@true"                     \
                                                                        \
    "\n MOVE LF@index LF@%1"                                            \
    "\n LABEL $substr$loop"                                             \
    "\n GT LF@lenCheck LF@index LF@%2"                                  \
    "\n JUMPIFEQ $substr$ret LF@lenCheck bool@true"                     \
    "\n GETCHAR LF@tmpChar LF@%1 LF@index"                              \
    "\n CONCAT LF@%retval LF@%retval LF@tmpChar"                        \
    "\n ADD LF@index LF@index int@1"                                    \
    "\n JUMP $substr$loop"                                              \
                                                                        \
    "\n LABEL $substr$errRet"                                           \
    "\n MOVE LF@%retval int@8"                                          \
                                                                        \
    "\n LABEL $substr$ret"                                              \
    "\n POPFRAME"                                                       \
    "\n RETURN"

// function ord(s : string, i : integer) : integer 
#define FUNCTION_ORD                                                    \
    "\n # Ord function"                                                 \
    "\n LABEL $ord"                                                     \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%retval"                                              \
    "\n MOVE LF@retval int@0"                                           \
    "\n DEFVAR LF@lenCheck"                                             \
    "\n DEFVAR LF@strLen"                                               \
    "\n STRLEN LF@strLen LF@%1"                                         \
    "\n GT LF@lenCheck int@1 LF@%2"                                     \
    "\n JUMPIFEQ $ord$ret LF@lenCheck bool@true"                        \
    "\n GT LF@lenCheck LF@%2 LF@strLen"                                 \
    "\n JUMPIFEQ $ord$ret LF@lenCheck bool@true"                        \
    "\n STRI2INT LF@%retval LF@%1 LF@%2"                                \
                                                                        \
    "\n LABEL $ord$ret"                                                 \
    "\n POPFRAME"                                                       \
    "\n RETURN"
    
// function chr(i : integer) : string
#define FUNCTION_CHR                                                    \
    "\n # Chr function"                                                 \
    "\n LABEL $chr"                                                     \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%retval"                                              \
    "\n DEFVAR LF@rngCheck"                                             \
    "\n EQ LF@rngCheck LF@%1 nil@nil"                                   \
    "\n JUMPIFEQ $chr$err LF@rngCheck bool@true"                        \
                                                                        \
    "\n MOVE LF@%retval string@"                                        \
    "\n LT LF@rngCheck LF@%1 int@0"                                     \
    "\n JUMPIFEQ $chr$ret LF@rngCheck bool@true"                        \
    "\n GT LF@rngCheck LF@%1 int@255"                                   \
    "\n JUMPIFEQ $chr$ret LF@rngCheck bool@true"                        \
    "\n INT2CHAR LF@%retval LF@%1"                                      \
    "\n JUMP $chr$ret"                                                  \
                                                                        \
    "\n LABEL $chr$err"                                                 \
    "\n MOVE LF@%retval int@8"                                          \
                                                                        \
    "\n LABEL $chr$ret"                                                 \
    "\n POPFRAME"                                                       \
    "\n RETURN"




bool cg_start ();

bool cg_function_header ( char *functionId );

bool cg_function_return ( char *functionId );

bool cg_process_data_type ( Data_type dataType );

bool cg_clear ();

bool cg_frame_to_pass_param (  );

bool cg_pass_param ( Token *token, int index );

bool cg_term ( Token *token );

bool cg_declare_var ( Item_data *item );

bool cg_define_var ( Item_data *item );

bool cg_push ( Token *token );

bool cg_if_header ( char *functionId );

bool cg_operation ( Data_type type, pt_rule rule );

#endif