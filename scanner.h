#ifndef SCANNER_H
#define SCANNER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "string_processor.h"

// For dynamic string memory allocation
#define DYNAMIC_STR_STARTING_MEM 20
#define DYNAMIC_STR_INCREASE_MEM 10


// Scanner states
typedef enum {

    SCANNER_STATE_START,                    // Starting state
    SCANNER_STATE_EOL,                      // EOL state
    SCANNER_STATE_TILD,                     // Tilda; encountering '=' leads to TT_NEQ
    SCANNER_STATE_FIRST_DASH,               // Might be either minus operator or start of comment
    SCANNER_STATE_SLASH,                    // Slash
    SCANNER_STATE_COMMENT_LINE,             // 
    SCANNER_STATE_COMMENT_READ,             // Skipping line comment
    SCANNER_STATE_COMMENT_LSB,              // First square bracket after double dash
    SCANNER_STATE_COMMENTBLOCK,             // Start of commentary block
    SCANNER_STATE_COMMENTBLOCK_EXIT,        // exit?
    SCANNER_STATE_INT,                      // Integer state; can be converted either to _DOUBLE after encountering _POINT or _EXP after encountering e(E) or remain _INT
    SCANNER_STATE_EXP,                      // e(E) found; can be converted either to 
    SCANNER_STATE_EXP_NUM,                  // exp -> num
    SCANNER_STATE_EXP_SIGN,                 // exp -> sign -> num
    SCANNER_STATE_POINT,                    // Point found, next state -- _DOUBLE
    SCANNER_STATE_DOUBLE,                   // Double type state
    SCANNER_STATE_HASHTAG,                  // Hashtag state
    SCANNER_STATE_LENGTH,                   // Get length state
    SCANNER_STATE_DOT,                      // Dot state -> conc. state
    SCANNER_STATE_CONC,                     // Concatenation state
    SCANNER_STATE_STRING,                   // String state
    SCANNER_STATE_ESC_SEQ,                  // Escape sequence state
    SCANNER_STATE_ESC_ZERO,                 // \0XX
    SCANNER_STATE_ESC_ONE,                  // \1XX
    SCANNER_STATE_ESC_TWO,                  // \2XX
    SCANNER_STATE_ESC_ZERO_ZERO,            // \00X
    SCANNER_STATE_ESC_TWO_FIVE,             // \25X
    SCANNER_STATE_ESC_OTHER,                // 
    SCANNER_STATE_ID,                       // Identifier state
    SCANNER_STATE_KW,                       // Keyword state
    SCANNER_STATE_DECL,                     // Declaration (colon) state
    SCANNER_STATE_MT,                       // More than state
    SCANNER_STATE_LT,                       // Less than state
    SCANNER_STATE_MET,                      // More or equal state
    SCANNER_STATE_LET,                      // Less or equal state
    SCANNER_STATE_EQUAL_SIGN,               // Equal sign -> '=' or "=="

} scannerState;

// Keywords
typedef enum {

    KW_INTEGER,             // integer
    KW_NUMBER,              // number
    KW_STRING,              // string
    KW_BOOLEAN,             // boolean
    KW_NIL,                 // nil
    KW_DO,                  // do
    KW_ELSE,                // else
    KW_END,                 // end 
    KW_FUNCTION,            // function
    KW_GLOBAL,              // global
    KW_IF,                  // if
    KW_LOCAL,               // local
    KW_REQUIRE,             // require
    KW_RETURN,              // return
    KW_THEN,                // then
    KW_WHILE,               // while
    // build-in functions
    KW_READS,               // reads 
    KW_READI,               // readi 
    KW_READN,               // readn 
    KW_WRITE,               // write
    KW_TOINTEGER,           // tointeger 
    KW_SUBSTR,              // substr
    KW_ORD,                 // ord
    KW_CHR,                 // chr

} Keyword;

// Token attribute
typedef union {

	int integer;            // Integer value.
	double floating;        // Value with floating point 
    Dynamic_string *string; // String or identifier value.
	Keyword keyword;        // If token is keyword

} Token_attribute;

// Types of token
typedef enum {

    TT_INT,                 // integer type
    TT_DOU,                 // double type
    TT_STR,                 // string type
    TT_BOO,                 // boolean type
    TT_NIL,                 // nil type
    TT_NDA,                 // not defined

    TT_IDE,                 // identifier type
    TT_KEY,                 // keyword type

    TT_LEN,                 // #
    TT_MUL,                 // *
    TT_DIV,                 // /
    TT_IDI,                 // //
    TT_ADD,                 // +
    TT_SUB,                 // -
    TT_CON,                 // ..
    TT_LTH,                 // less than
    TT_LET,                 // less or equal than
    TT_MTH,                 // more than
    TT_MET,                 // more or equal than
    TT_EQU,                 // equal
    TT_NEQ,                 // not equal
    TT_IEQ,                 // equal for if conditions

    TT_LBR,                 // (
    TT_RBR,                 // )
    TT_COL,                 // :
    TT_COM,                 // ,

    TT_EOF,                 // end of file
    
} Token_type;
// Token structure
typedef struct {

    Token_type type;
    Token_attribute attribute;

} Token;

/** Main function of scanner, scans tokens, changes their parameters, sends them further
 * @param token token pointer
 * @return 0 in case the token is ok,
 *         1 in case of lexical error,
 *         99 in case of internal error
 */
int get_next_token ( Token *token );

#endif