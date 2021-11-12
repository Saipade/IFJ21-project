/*

*/

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "scanner.h"
#include "symtable.h"

typedef struct {

    Token token;            // token got by get_next_token func

    Sym_table symTable[10]; // 
    Token tokenTable[10];   // for multiple assignment

    Item_data *lhsId;       // left-hand  side func/var identifier
    Item_data *rhsId;       // right-hand side func/var identifier
    Item_data *auxId;       // auxiliary identifier
    Item_data *currentId;   // current func/var identifier

    bool inDecl;            // if parser is in declaration
    bool inFunc;            // if parser is in function
    bool inLoop;            // if parser is in if-statement/while cycle

    int currentDepth;       // depth for local symtables
    int whereAmI;           // function declaration or function definition
    int paramIndex;
    int labelDepth;
    int idCounter;

} Parser_data;

bool parser_data_init ( Parser_data *parserData );
/**
 * Function applying rule
 * "<program> -> require "ifj21".<functions list>"
 * Called only once per file
 */ 
int parse (  );
int rule_prologue ( Parser_data *parserData );
int rule_functionList ( Parser_data *parserData );
int rule_paramList ( Parser_data *parserData );
int rule_statementList ( Parser_data *parserData );
int rule_Write ( Parser_data *parserData );
int rule_Value ( Parser_data *parserData );
int rule_Expression ( Parser_data *parserData );
int rule_expressionList ( Parser_data *parserData );



#endif