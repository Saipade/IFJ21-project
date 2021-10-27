/*

*/

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "scanner.h"
#include "symtable.h"

typedef struct {

    Token token;           // token got by get_next_token func

    Sym_table symTable[10];
    //Sym_table localTable;  // local  symbol table
    //Sym_table globalTable; // global symbol table

    Item_data *lhsId;      // left-hand  side func/var identifier
    Item_data *rhsId;      // right-hand side func/var identifier
    Item_data *currentId;  // current func/var identifier

    bool if_decl;          // if function was declared
    bool in_decl;          // if parser is in declaration
    bool in_func;          // if parser is in function
    bool in_loop;          // if parser is in if-statement/while cycle

    int currentDepth;     // depth for local symtables
    int whereAmI;         // function declaration or function definition
    int labelId;
    int labelDepth;
    int paramId;

} Parser_data;

bool parser_data_init ( Parser_data *parserData );
/**
 * Function applying rule
 * "<program> -> require "ifj21".<functions list>"
 * Called only once per file
 */ 
int parse (  );
int rule_prologue ( Parser_data *parserData );
int rule_functionDeclaration ( Parser_data *parserData );
int rule_functionList ( Parser_data *parserData );
int rule_paramList ( Parser_data *parserData );
/**
 * Function applying rules
 * "<declaration part> -> <declaration list>"
 * "<declaration list> -> <declaration> <declaration list>"
 * "<declaration list> -> <declaration>"
 * "<declaration> -> local "
 */
int rule_declarationPart ( Parser_data *parserData );

#endif
