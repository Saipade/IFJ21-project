#ifndef SCANNER_H
#define SCANNER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "string_processor.h"

// Scanner states
#define SCANNER_STATE_START 0                   // Starting state
#define SCANNER_STATE_EOL 1                     // EOL state
#define SCANNER_STATE_TILD 2                    // Tilda; encountering '=' leads to TT_NEQ
#define SCANNER_STATE_FIRST_DASH 3              // Might be either minus operator or start of comment
#define SCANNER_STATE_SLASH 4                   // Slash
#define SCANNER_STATE_COMMENT_LINE 5            // 
#define SCANNER_STATE_COMMENT_READ 6            // Skipping line comment
#define SCANNER_STATE_COMMENT_LSB 7             // First square bracket after double dash
#define SCANNER_STATE_COMMENTBLOCK 8            // Start of commentary block
#define SCANNER_STATE_COMMENTBLOCK_EXIT 9       // exit?
#define SCANNER_STATE_INT 10                    // Integer state; can be converted either to _DOUBLE after encountering _POINT or _EXP after encountering e(E) or remain _INT
#define SCANNER_STATE_EXP 11                    // e(E) found; can be converted either to 
#define SCANNER_STATE_EXP_NUM 12                // exp -> num
#define SCANNER_STATE_EXP_SIGN 13               // exp -> sign -> num
#define SCANNER_STATE_POINT 14                  // Point found, next state -- _DOUBLE
#define SCANNER_STATE_DOUBLE 15                 // Double type state
#define SCANNER_STATE_HASHTAG 16                // Hashtag state
#define SCANNER_STATE_LENGTH 17                 // Get length state
#define SCANNER_STATE_DOT 18                    // Dot state -> conc. state
#define SCANNER_STATE_CONC 19                   // Concatenation state
#define SCANNER_STATE_STRING 20                 // String state
#define SCANNER_STATE_ESC_SEQ 21                // Escape sequence state
#define SCANNER_STATE_ESC_ZERO 22               // \0XX
#define SCANNER_STATE_ESC_ONE 23                // \1XX
#define SCANNER_STATE_ESC_TWO 24                // \2XX
#define SCANNER_STATE_ESC_ZERO_ZERO 25          // \00X
#define SCANNER_STATE_ESC_TWO_FIVE 26           // \25X
#define SCANNER_STATE_ESC_OTHER 28              // 
#define SCANNER_STATE_ID 29                     // Identifier state
#define SCANNER_STATE_KW 30                     // Keyword state
#define SCANNER_STATE_DECL 31                   // Declaration (colon) state
#define SCANNER_STATE_MT 32                     // More than state
#define SCANNER_STATE_LT 33                     // Less than state
#define SCANNER_STATE_MET 34                    // More or equal state
#define SCANNER_STATE_LET 35                    // Less or equal state
// TODO : ADD MORE STATES
#define DYNAMIC_STR_STARTING_MEM 20
#define DYNAMIC_STR_INCREASE_MEM 10



// Keywords
typedef enum {

    KW_DO,                  // do
    KW_ELSE,                // else
    KW_END,                 // end 
    KW_FUNCTION,            // function
    KW_GLOBAL,              // global
    KW_IF,                  // if
    KW_INTEGER,             // integer
    KW_LOCAL,               // local
    KW_NIL,                 // nil
    KW_NUMBER,              // number
    KW_REQUIRE,             // require
    KW_RETURN,              // return
    KW_STRING,              // string
    KW_THEN,                // then
    KW_WHILE,               // while

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

    TT_IDE,                 // identifier type
    TT_KEY,                 // keyword type

    TT_INT,                 // integer type
    TT_DOU,                 // double type
    TT_STR,                 // string type
    TT_NIL,                 // nil type

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

    TT_LBR,                 // (
    TT_RBR,                 // )
    TT_COL,                 // :
    TT_COM,                 // ,

    TT_EOL,                 // end of line
    TT_EOF,                 // end of file
    TT_EMPTY,               // empty token
    
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
