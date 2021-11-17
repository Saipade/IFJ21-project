/*

*/


/*
    Errors list for different parts of compiler
*/
#define ERR_LEXICAL 1
#define ERR_SYNTAX 2
#define ERR_SEMANTIC_UNDEF_VAR 3
#define ERR_SEMANTIC_INCOP_TYPE 4 
#define ERR_SEMANTIC_FUNC_PAR 5
#define ERR_SEMANTIC_INCOP_EXP 6
#define ERR_SEMANTIC_OTHER 7
#define ERR_NIL 8
#define ERR_DIV_NUL 9
#define ERR_INTERNAL 99
#define SCAN_OK 0


/*1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
• 2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu).
• 3 - sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefinici proměnné, atp.
• 4 - sémantická chyba v příkazu přiřazení (typová nekompatibilita).
• 5 - sémantická chyba v programu – špatný počet/typ parametrů či návratových hodnot
u volání funkce či návratu z funkce.
• 6 - sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních
výrazech.
• 7 - ostatní sémantické chyby.
• 8 - běhová chyba při práci s neočekávanou hodnotou nil.
• 9 - běhová chyba celočíselného dělení nulovou konstantou*/