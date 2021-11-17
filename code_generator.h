/*

*/

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include"string_processor.h"
#include"scanner.h"

//Dynamic_string *codeString;



// Append string to result code


// Definition of built-in functions
// TOTEST
// function reads() : string
#define FUNCTION_READS                                                  \
    "\n # Reads function"                                               \
    "\n LABEL $reads"                                                   \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%retString"                                           \
    "\n DEFVAR LF@tmpChar"                                              \
    "\n DEFVAR LF@nlCheck"                                              \
    "\n MOVE LF@%retString string@"                                     \
    "\n MOVE LF@tmpChar string@"                                        \
                                                                        \
    "\n LABEL $reads$reading"                                           \
    "\n CONCAT LF@%retString LF@%retString LF@tmpChar"                  \
    "\n READ LF@tmpChar string"                                         \
    "\n EQ LF@nlCheck LF@tmpChar string@10"                             \
    "\n JUMPIFEQ $reads$reading LF@nlCheck bool@false"                  \
                                                                        \
    "\n LABEL $reads$ret"                                               \
    "\n POPFRAME"                                                       \
    "\n RETURN"                                                 
// TODO
// function readi() : integer
#define FUNCTION_READI                                                  \
    "\n # Readi function"                                               \
    "\n LABEL $readi"                                                   \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%retInteger"                                          \
    "\n DEFVAR LF@tmpChar"                                              \
    "\n DEFVAR LF@tmpString"                                            \
    "\n DEFVAR LF@irrsignCheck"                                         \
                                                                        \
    "\n LABEL $readi$reading"                                           \
    "\n CONCAT LF@tmpString LF@tmpString LF@tmpChar"                    \
    "\n LABEL $readi$ignoretpaceReading"                                \
    "\n READ LF@tmpChar string"                                         \
    "\n EQ LF@irrsignCheck LF@tmpChar string@32"                        \
    "\n JUMPIFEQ $readi$ignoretpaceReading LF@irrsignCheck bool@true"   \
                                                                        \
    "\n LABEL $readi$convert"                                           \
    "\n "                                                               \
    "\n LABEL $readi$ret"                                               \
    "\n POPFRAME"                                                       \
    "\n RETURN"                                         
// TODO
// function readn() : number
#define FUNCTION_READN                                                  \
    "\n # Readn function"                                               \
    "\n LABEL $readn"                                                   \
    "\n PUSHFRAME"                                                      \
    "\n POPFRAME"                                                       \
    "\n RETURN"
// TODO
// function write
#define FUNCTION_WRITE                                                  \
    "\n # Write function"                                               \
    "\n LABEL $write"                                                   \
    "\n PUSHFRAME"                                                      \
    "\n POPFRAME"                                                       \
    "\n RETURN"
// TODO
// function tointeger(f : number) : integer
#define FUNCTION_TOINTEGER                                              \
    "\n # Tointeger function"                                           \
    "\n LABEL $tointeger"                                               \
    "\n PUSHFRAME"                                                      \
    "\n POPFRAME"                                                       \
    "\n RETURN"
// TODO//TOTEST
// function substr(s : string, i : number, j : number) : string
#define FUNCTION_SUBSTR                                                 \
    "\n # Substr function"                                              \
    "\n LABEL $substr"                                                  \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%retString"                                           \
    "\n DEFVAR LF@tmpChar"                                              \
    "\n DEFVAR LF@typeCheck"                                            \
    "\n DEFVAR LF@lngCheck"                                             \
    "\n DEFVAR LF@strLen"                                               \
    "\n DEFVAR LF@index"                                                \
    "\n MOVE LF@%retString string@"                                     \
    "\n STRLEN LF@strLen LF@%0"                                         \
                                                                        \
    "\n EQ LF@typeCheck LF@%0 nil@nil"                                  \
    "\n JUMPIFEQ $substr$errRet LF@typeCheck bool@true"                 \
    "\n EQ LF@typeCheck LF@%1 nil@nil"                                  \
    "\n JUMPIFEQ $substr$errRet LF@typeCheck bool@true"                 \
    "\n EQ LF@typeCheck LF@%2 nil@nil"                                  \
    "\n JUMPIFEQ $substr$errRet LF@typeCheck bool@true"                 \
                                                                        \
    "\n GT LF@lngCheck int@1 LF@%1"                                     \
    "\n JUMPIFEQ $substr$ret LF@lngCheck bool@true"                     \
    "\n GT LF@lngCheck LF@%1 LF@strLen"                                 \
    "\n JUMPIFEQ $substr$ret LF@lngCheck bool@true"                     \
    "\n GT LF@lngCheck int@1 LF@%2"                                     \
    "\n JUMPIFEQ $substr$ret LF@lngCheck bool@true"                     \
    "\n GT LF@lngCheck LF@%2 LF@strLen"                                 \
    "\n JUMPIFEQ $substr$ret LF@lngCheck bool@true"                     \
                                                                        \
    "\n MOVE LF@index LF@%1"                                            \
    "\n LABEL $substr$loop"                                             \
    "\n GT LF@lngCheck LF@index LF@%2"                                  \
    "\n JUMPIFEQ $substr$ret LF@lngCheck bool@true"                     \
    "\n GETCHAR LF@tmpChar LF@%0 LF@index"                              \
    "\n CONCAT LF@%retString LF@%retString LF@tmpChar"                  \
    "\n ADD LF@index LF@index int@1"                                    \
    "\n JUMP $substr$loop"                                              \
                                                                        \
    "\n LABEL $substr$errRet"                                           \
    "\n MOVE LF@%retString int@8"                                       \
                                                                        \
    "\n LABEL $substr$ret"                                              \
    "\n POPFRAME"                                                       \
    "\n RETURN"
// TOTEST
// function ord(s : string, i : integer) : integer 
#define FUNCTION_ORD                                                    \
    "\n # Ord function"                                                 \
    "\n LABEL $ord"                                                     \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%retString"                                           \
    "\n DEFVAR LF@lngCheck"                                             \
    "\n DEFVAR LF@strLen"                                               \
    "\n STRLEN LF@strLen LF@%0"                                         \
    "\n MOVE LF@retString string@"                                      \
    "\n GT LF@lngCheck int@1 LF@%1"                                     \
    "\n JUMPIFEQ $ord$ret LF@lngCheck bool@true"                        \
    "\n GT LF@lngCheck LF@%1 LF@strLen"                                 \
    "\n JUMPIFEQ $ord$ret LF@lngCheck bool@true"                        \
    "\n STRI2INT LF@%retString LF@%0 LF@%1"                             \
                                                                        \
    "\n LABEL $ord$ret"                                                 \
    "\n POPFRAME"                                                       \
    "\n RETURN"
// TOTEST
// function chr(i : integer) : string
#define FUNCTION_CHR                                                    \
    "\n # Chr function"                                                 \
    "\n LABEL $chr"                                                     \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%retInteger"                                          \
    "\n DEFVAR LF@rngCheck"                                             \
    "\n MOVE LF@%retInteger string@"                                    \
    "\n LT LF@rngCheck LF@%0 int@0"                                     \
    "\n JUMPIFEQ $chr$ret LF@rngCheck bool@true"                        \
    "\n GT LF@rngCheck LF@%0 int@255"                                   \
    "\n JUMPIFEQ $chr$ret LF@rngCheck bool@true"                        \
    "\n INT2CHAR LF@%retInteger LF@%0"                                  \
                                                                        \
    "\n LABEL $chr$ret"                                                 \
    "\n POPFRAME"                                                       \
    "\n RETURN"




bool cg_start ();

void _code_string ( Dynamic_string *string );

bool cg_function_output_type ( Data_type dataType, int index );

bool cg_function_input_type ( char *inputId, Data_type dataType, int index );

bool cg_function_header ( char *functionId );

bool cg_function_return ( char *functionId );

bool cg_process_data_type ( Data_type dataType );

bool cg_clear ();

#endif