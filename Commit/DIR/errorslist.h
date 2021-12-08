/**
 * PROJECT: Implementation of imperative IFJ21 programming language compiler
 * 
 * AUTHOR(S): Maksim Tikhonov (xtikho00)
 *            Sadovskyi Dmytro (xsadov06)
 */

// 1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
#define ERR_LEXICAL 1   
// 2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu).            
#define ERR_SYNTAX 2
// 3 - sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefinici proměnné, atp.
#define ERR_SEMANTIC_UNDEF_VAR 3
// 4 - sémantická chyba v příkazu přiřazení (typová nekompatibilita).
#define ERR_SEMANTIC_INCOP_TYPE 4 
// 5 - sémantická chyba v programu – špatný počet/typ parametrů či návratových hodnot u volání funkce či návratu z funkce.
#define ERR_SEMANTIC_FUNC_PAR 5
// 6 - sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních
#define ERR_SEMANTIC_INCOP_EXP 6
// 7 - ostatní sémantické chyby.
#define ERR_SEMANTIC_OTHER 7
// 8 - běhová chyba při práci s neočekávanou hodnotou nil.
#define ERR_NIL 8
// 9 - běhová chyba celočíselného dělení nulovou konstantou
#define ERR_DIV_NUL 9
#define ERR_INTERNAL 99
#define SCAN_OK 0

