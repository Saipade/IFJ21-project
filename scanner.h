#include <stdlib.h>
#include <string.h>

enum keyword {
    KW_DO,         // do
    KW_ELSE,       // else
    KW_END,        // end 
    KW_FUNC,       // function
    KW_GLOB,       // global
    KW_IF,         // if
    KW_INT,        // integer
    KW_LOCAL,      // local
    KW_NIL,        // nil
    KW_NUM,        // number
    KW_REQ,        // require
    KW_RET,        // return
    KW_STR,        // string
    KW_THEN,       // then
    KW_WHILE,      // while
};

enum token_type {

    TT_INT, // integer type
    TT_NUM, // numeric type
    TT_STR, // string type 

    TT_MTH, // more than
    TT_MET, // more or equal than
    TT_LTH, // less than
    TT_LET, // less or equal than
    TT_NEQ, // not equal

};