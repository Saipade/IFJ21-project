/*

*/

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Definition of built-in functions
// TOTEST
// function reads() : string
#define FUNCTION_READS                                                  \
    "\n # Reads function"                                               \
    "\n LABEL $reads"                                                   \
    "\n PUSHFRAME"                                                      \
    "\n DEFVAR LF@%resString"                                           \
    "\n DEFVAR LF@tmpChar"                                              \
    "\n DEFVAR LF@nlCheck"                                              \
    "\n MOVE LF@%resString string@"                                     \
    "\n MOVE LF@tmpChar string@"                                        \
                                                                        \
    "\n LABEL $reads$reading"                                           \
    "\n CONCAT LF@%resString LF@%resString LF@tmpChar"                  \
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
    "\n DEFVAR LF@%resInteger"                                          \
    "\n DEFVAR LF@tmpChar"                                              \
    "\n DEFVAR LF@tmpString"                                            \
    "\n DEFVAR LF@irrsignCheck"                                         \
                                                                        \
    "\n LABEL $readi$reading"                                           \
    "\n CONCAT LF@tmpString LF@tmpString LF@tmpChar"                    \
    "\n LABEL $readi$ignorespaceReading"                                \
    "\n READ LF@tmpChar string"                                         \
    "\n EQ LF@irrsignCheck LF@tmpChar string@32"                        \
    "\n JUMPIFEQ $readi$ignorespaceReading LF@irrsignCheck bool@true"   \
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
    "\n DEFVAR LF@%resString"                                           \
    "\n DEFVAR LF@tmpChar"                                              \
    "\n DEFVAR LF@typeCheck"                                            \
    "\n DEFVAR LF@lngCheck"                                             \
    "\n DEFVAR LF@strLen"                                               \
    "\n DEFVAR LF@index"                                                \
    "\n MOVE LF@%resString string@"                                     \
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
    "\n CONCAT LF@%resString LF@%resString LF@tmpChar"                  \
    "\n ADD LF@index LF@index int@1"                                    \
    "\n JUMP $substr$loop"                                              \
                                                                        \
    "\n LABEL $substr$errRet"                                           \
    "\n MOVE LF@%resString int@8"                                       \
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
    "\n DEFVAR LF@%resString"                                           \
    "\n DEFVAR LF@lngCheck"                                             \
    "\n DEFVAR LF@strLen"                                               \
    "\n STRLEN LF@strLen LF@%0"                                         \
    "\n MOVE LF@resString string@"                                      \
    "\n GT LF@lngCheck int@1 LF@%1"                                     \
    "\n JUMPIFEQ $ord$ret LF@lngCheck bool@true"                        \
    "\n GT LF@lngCheck LF@%1 LF@strLen"                                 \
    "\n JUMPIFEQ $ord$ret LF@lngCheck bool@true"                        \
    "\n STRI2INT LF@%resString LF@%0 LF@%1"                             \
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
    "\n DEFVAR LF@%resInteger"                                          \
    "\n DEFVAR LF@rngCheck"                                             \
    "\n MOVE LF@%resInteger string@"                                    \
    "\n LT LF@rngCheck LF@%0 int@0"                                     \
    "\n JUMPIFEQ $chr$ret LF@rngCheck bool@true"                        \
    "\n GT LF@rngCheck LF@%0 int@255"                                   \
    "\n JUMPIFEQ $chr$ret LF@rngCheck bool@true"                        \
    "\n INT2CHAR LF@%resInteger LF@%0"                                  \
                                                                        \
    "\n LABEL $chr$ret"                                                 \
    "\n POPFRAME"                                                       \
    "\n RETURN"




void cg_start ();





bool cg_clear ();

#endif
