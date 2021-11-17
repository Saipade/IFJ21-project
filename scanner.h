/*

*/

#ifndef SCANNER_H
#define SCANNER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "string_processor.h"

// Scanner states
typedef enum {

    SCANNER_STATE_START,                    // Starting state
    SCANNER_STATE_EOL,                      // EOL state
    SCANNER_STATE_TILD,                     // Tilda; encountering '=' leads to T_NEQ
    SCANNER_STATE_FIRST_DASH,               // Might be either minus operator or start of comment
    SCANNER_STATE_SLASH,                    // Slash
    SCANNER_STATE_COMMENT_LINE,             // --
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

} scanner_state;

// Keywords
typedef enum {

    KW_INTEGER,                             // integer
    KW_NUMBER,                              // number
    KW_STRING,                              // string
    //KW_BOOLEAN,                           // boolean
    KW_NIL,                                 // nil
    KW_DO,                                  // do
    KW_ELSE,                                // else
    KW_END,                                 // end 
    KW_FUNCTION,                            // function
    KW_GLOBAL,                              // global
    KW_IF,                                  // if
    KW_LOCAL,                               // local
    KW_REQUIRE,                             // require
    KW_RETURN,                              // return
    KW_THEN,                                // then
    KW_WHILE,                               // while
    // build-in functions
    KW_READS,                               // reads 
    KW_READI,                               // readi 
    KW_READN,                               // readn 
    KW_WRITE,                               // write
    KW_TOINTEGER,                           // tointeger 
    KW_SUBSTR,                              // substr
    KW_ORD,                                 // ord
    KW_CHR,                                 // chr

} Keyword;

// Token attribute
typedef union {

	int integer;                            // Integer value.
	double floating;                        // Value with floating point 
    Dynamic_string *string;                 // String or identifier value.
	Keyword keyword;                        // If token is keyword

} Token_attribute;

// Types of token
typedef enum {

    T_INT,                                  // integer type
    T_NUM,                                  // double type
    T_STR,                                  // string type
    T_BOO,                                  // boolean type
    T_NIL,                                  // nil type
    T_NDA,                                  // not defined

    T_IDE,                                  // identifier type
    T_KEY,                                  // keyword type

    T_ADD,                                  // +
    T_SUB,                                  // -
    T_MUL,                                  // *
    T_DIV,                                  // /
    T_IDI,                                  // //
    T_CAT,                                  // ..
    T_LEN,                                  // #

    T_LTH,                                  // <
    T_LET,                                  // <=
    T_MTH,                                  // >
    T_MET,                                  // >=
    T_EQU,                                  // ==
    T_NEQ,                                  // ~=

    T_LBR,                                  // (
    T_RBR,                                  // )

    T_DOL,                                  // $
//  EXPRESSION SYMBOLS END
    T_ASS,                                  // =
    T_COL,                                  // :
    T_COM,                                  // ,

    T_EOF,                                  // end of file
    
} Data_type;
// Token structure
typedef struct {

    Data_type type;
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