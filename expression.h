
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "parser.h"

#define IS_I(ITEM)									\
	   ITEM->symbol == INT						\
	|| ITEM->symbol == NUM						\
	|| ITEM->symbol == STR					    \
	|| ITEM->symbol == NIL                        \
    || ITEM->symbol == IDE

typedef enum {

    S,                  // shift
    R,                  // reduce
    E,                  // equal
    B,                  // blank -- not defined

} pc_operation;

typedef enum {

    INT,                // integer
    NUM,                // number
    STR,                // string
    NIL,                // nil

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

    LBR,
    RBR,

    STOP,               // stop sign
    DOL,                // $

} pt_terminal;

typedef enum {

    NT_RULE,            // E -> i

    LEN_NT,             // E -> # E

    NT_PLUS_NT,         // E -> E + E
    NT_MINUS_NT,        // E -> E - E
    NT_MUL_NT,          // E -> E * E
    NT_DIV_NT,          // E -> E / E
    NT_IDIV_NT,         // E -> E // E
    NT_CAT_NT,          // E -> E .. E

    NT_LTH_NT,          // E -> E < E
    NT_LET_NT,          // E -> E <= E
    NT_MTH_NT,          // E -> E > E
    NT_MET_NT,          // E -> E >= E
    NT_IEQ_NT,          // E -> E == E
    NT_NEQ_NT,          // E -> E ~= E

    LBR_NT_RBR,         // E -> ( E )

    ND_RULE,

} pt_rule;

typedef enum {

    I_PLUSMINUS,
    I_MULDIV,
    I_CMP,
    I_LBR,
    I_RBR,
    I_i,
    I_HASH,
    I_CAT,
    I_DOLLAR,

} pt_index;


#endif