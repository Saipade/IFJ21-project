#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "parser.h"

#define IS_I(SYMBOL)						    \
	   SYMBOL == INT						    \
	|| SYMBOL == NUM						    \
	|| SYMBOL == STR					        \
	|| SYMBOL == NIL                            \
    || SYMBOL == IDE


typedef enum {

    S,                  // shift
    R,                  // reduce
    E,                  // equal
    B,                  // blank -- not defined

} pt_operation;

typedef enum {

    INT,                // integer
    NUM,                // number
    STR,                // string
    BOO,                // boolean  
    NIL,                // nil

    NDA,                // not defined

    IDE,                // identifier
    NONTERM,            // non-terminal

    ADD,                // +
    SUB,                // -
    MUL,                // *
    DIV,                // /
    IDI,                // //
    CAT,                // ..
    LEN,                // #

    LTH,                // <
    LET,                // <=
    MTH,                // >
    MET,                // >= 
    EQU,                // ==
    NEQ,                // ~=

    LBR,                // (
    RBR,                // )

    STOP,               // stop sign
    DOL,                // $

} pt_terminal;

typedef enum {

    E_RULE,            // E -> i

    LEN_E,             // E -> # E

    E_PLUS_E,         // E -> E + E
    E_MINUS_E,        // E -> E - E
    E_MUL_E,          // E -> E * E
    E_DIV_E,          // E -> E / E
    E_IDIV_E,         // E -> E // E
    E_CAT_E,          // E -> E .. E

    E_LTH_E,          // E -> E < E
    E_LET_E,          // E -> E <= E
    E_MTH_E,          // E -> E > E
    E_MET_E,          // E -> E >= E
    E_EQU_E,          // E -> E == E
    E_NEQ_E,          // E -> E ~= E

    LBR_E_RBR,         // E -> ( E )

    ND_RULE,

} pt_rule;

typedef enum {

    I_PLUSMINUS,
    I_MULDIV,
    I_CMP,
    I_LBR,
    I_RBR,
    I_i,
    I_LEN,
    I_CAT,
    I_DOLLAR,

} pt_index;





#endif