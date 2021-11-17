/*

*/
#ifndef SCANNER_C
#define SCANNER_C

#include "errorslist.h"
#include "scanner.h"
#include "string_processor.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

FILE *srcF;


// Dynamic string for token               
Dynamic_string *scannerString;

/** Pre-return number processing
 * @param str dynamic string
 * @param token pointer to token
 * @return 0 in case the token is ok
 */
void set_source_file(FILE* file){
    srcF = file;
}

int _integer_or_number ( Dynamic_string *str, Token *token ) {

    char *ptr;
    
    if (token->type == T_INT) {

        int tmp = (int) strtol( str->str, &ptr, 10 );
        token->attribute.integer = tmp;
        ds_free( str );
        return SCAN_OK;

    } 
    
    else if (token->type == T_DOU) {

        double tmp = strtod( str->str, &ptr );
        token->attribute.floating = tmp;
        ds_free( str );
        return SCAN_OK;
        
    }

}
/** Pre-return identifier/keyword processing
 * @param str dynamic string
 * @param token pointer to token
 * @return 0 in case the token is ok
 *         99 in case of internal error
 */
int _keyword_or_id ( Dynamic_string *str, Token *token ) {

    if      (strcmp( str->str, "integer" ) == 0) token->attribute.keyword = KW_INTEGER;
    else if (strcmp( str->str, "number" ) == 0) token->attribute.keyword = KW_NUMBER;
    else if (strcmp( str->str, "string" ) == 0) token->attribute.keyword = KW_STRING;
    else if (strcmp( str->str, "boolean" ) == 0) token->attribute.keyword = KW_BOOLEAN;
    else if (strcmp( str->str, "nil" ) == 0) token->attribute.keyword = KW_NIL;
    else if (strcmp( str->str, "do" ) == 0) token->attribute.keyword = KW_DO;
    else if (strcmp( str->str, "else" ) == 0) token->attribute.keyword = KW_ELSE;
    else if (strcmp( str->str, "end" ) == 0) token->attribute.keyword = KW_END;
    else if (strcmp( str->str, "function" ) == 0) token->attribute.keyword = KW_FUNCTION;
    else if (strcmp( str->str, "global" ) == 0) token->attribute.keyword = KW_GLOBAL;
    else if (strcmp( str->str, "if" ) == 0) token->attribute.keyword = KW_IF;
    else if (strcmp( str->str, "local" ) == 0) token->attribute.keyword = KW_LOCAL;
    else if (strcmp( str->str, "require" ) == 0) token->attribute.keyword = KW_REQUIRE;
    else if (strcmp( str->str, "return" ) == 0) token->attribute.keyword = KW_RETURN;
    else if (strcmp( str->str, "then" ) == 0) token->attribute.keyword = KW_THEN;
    else if (strcmp( str->str, "while" ) == 0) token->attribute.keyword = KW_WHILE;

    else if (strcmp( str->str, "reads" ) == 0) token->attribute.keyword = KW_READS;
    else if (strcmp( str->str, "raedi" ) == 0) token->attribute.keyword = KW_READI;
    else if (strcmp( str->str, "readn" ) == 0) token->attribute.keyword = KW_READN;
    else if (strcmp( str->str, "write" ) == 0) token->attribute.keyword = KW_WRITE;
    else if (strcmp( str->str, "tointeger" ) == 0) token->attribute.keyword = KW_TOINTEGER;
    else if (strcmp( str->str, "substr" ) == 0) token->attribute.keyword = KW_SUBSTR;
    else if (strcmp( str->str, "ord" ) == 0) token->attribute.keyword = KW_ORD;
    else if (strcmp( str->str, "chr" ) == 0) token->attribute.keyword = KW_CHR;
    
    else token->type = T_IDE;

    if (token->type == T_IDE) {
        
        if (!ds_copy( str, token->attribute.string )) {
            ds_free( str );
            return ERR_INTERNAL;
        }

        ds_free( str );
        return SCAN_OK;
        
    }
    
    else {

        token->type = T_KEY;
        ds_free( str );
        return SCAN_OK;

    }

}

void _scanner_string ( Dynamic_string *string ) {

	scannerString = string;

}

int get_next_token ( Token *token ) {

    if (!srcF || !scannerString) {
        return ERR_INTERNAL;
    }
    int scannerState = SCANNER_STATE_START;
    
    token->type = T_NDA;
    token->attribute.string = scannerString;

    Dynamic_string str;
    Dynamic_string *scannerString = &str;
    if (!ds_init( scannerString )) return ERR_INTERNAL;
    
    char ESstr[3], c;
    
    while (true) {

        c = (char) getc( srcF );

        switch (scannerState) {

            case (SCANNER_STATE_START):

                if (c == ' ' || c == '\n' || c == '\t') {
                    scannerState = SCANNER_STATE_START;
                }

                else if (c >= '1' && c <= '9') {
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                    scannerState = SCANNER_STATE_INT;
                }

                else if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                    scannerState = SCANNER_STATE_ID;
                } 
                
                else if (c == '>') {
                    scannerState = SCANNER_STATE_MT;
                }
                
                 else if (c == '<') {
                    scannerState = SCANNER_STATE_LT;
                } 

                else if (c == '+') { 
                    ds_free( scannerString );
                    token->type = T_ADD;
                    return SCAN_OK;
                } 

                else if (c == '-') {
                    scannerState = SCANNER_STATE_FIRST_DASH;
                } 

                else if (c == '*') { 
                    ds_free( scannerString );
                    token->type = T_MUL;
                    return SCAN_OK;
                } 

                else if (c == '/') { 
                    scannerState = SCANNER_STATE_SLASH;

                } 

                else if (c == '=') { 
                    scannerState = SCANNER_STATE_EQUAL_SIGN;
                    ds_free( scannerString );
                    token->type = T_EQU;
                    return SCAN_OK;
                }

                else if (c == '~') {
                    scannerState = SCANNER_STATE_TILD;
                }
                
                else if (c == ':') { 
                    ds_free( scannerString );
                    token->type = T_COL;
                    return SCAN_OK;
                }

                else if (c == ',') { 
                    ds_free( scannerString );
                    token->type = T_COM;
                    return SCAN_OK;
                }
                
                else if (c == '(') { 
                    ds_free( scannerString );
                    token->type = T_LBR;
                    return SCAN_OK;
                }
                
                else if (c == ')') { 
                    ds_free( scannerString );
                    token->type = T_RBR;
                    return SCAN_OK;
                }
                
                else if (c == '#') { 
                    ds_free( scannerString );
                    token->type = T_LEN;
                }

                else if (c == '.') {
                    scannerState = SCANNER_STATE_DOT;
                }
                
                else if (c == '"') {
                    scannerState = SCANNER_STATE_STRING;
                }

                else if (c == EOF) { 
                    ds_free( scannerString );
                    token->type = T_EOF;
                    return SCAN_OK;
                }

                else {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }
                
            break;

            // NUMBER PROCESSING
            case (SCANNER_STATE_INT):

                if (c >= '0' && c <= '9') {
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                } 
                
                else if (c == '.') {
                    scannerState = SCANNER_STATE_POINT;
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                } 
                
                else if (c == 'E' || c == 'e') {
                    scannerState = SCANNER_STATE_EXP;
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                }

                else { 
                    ungetc( c, srcF );
                    token->type = T_INT;
                    return _integer_or_number( scannerString, token );
                }

            break;
            
            case (SCANNER_STATE_POINT):

                if (c >= '0' && c <= '9') {
                    scannerState = SCANNER_STATE_DOUBLE;
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                }

                else {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }

            break;

            case (SCANNER_STATE_DOUBLE):

                if(c >= '0' && c <= '9') {
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                } 

                else if (c == 'e' && c == 'E') {
                    scannerState = SCANNER_STATE_EXP;
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                }

                else { 
                    ungetc( c, srcF );
                    token->type = T_DOU;
                    return _integer_or_number( scannerString, token );
                }

            break;

            case (SCANNER_STATE_EXP):

                if (c >= '0' && c <= '9') {
                    scannerState = SCANNER_STATE_EXP_NUM;
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                }

                else if (c == '+' || c == '-') {
                    scannerState = SCANNER_STATE_EXP_SIGN;
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                }

                else {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }
            
            break;

            case (SCANNER_STATE_EXP_NUM): 

                if(c >= '0' && c <= '9') {
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                }

                else {
                    ungetc( c, srcF );
                    token->type = T_DOU;
                    return _integer_or_number( scannerString, token );
                }
                
            break;

            case (SCANNER_STATE_EXP_SIGN):

                if(c >= '0' && c <= '9') {
                    scannerState = SCANNER_STATE_EXP_NUM;
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                }

                else {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }

            break;

            // IDENTIFIER OR KEYWORD PROCESSING
            case (SCANNER_STATE_ID):

                if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c > '0' && c < '9')) {
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                } 

                else { 
                    ungetc( c, srcF );
                    return _keyword_or_id( scannerString, token );
                }
                
            break;

            // 2-SIGNED RELATIONSHIPS OPERATORS
            case (SCANNER_STATE_MT):

                if (c == '=') { 
                    token->type = T_MET;
                }

                else { 
                    ungetc( c, srcF );
                    token->type = T_MTH;
                    
                }

                ds_free( scannerString );
                return SCAN_OK;

            break;

            case (SCANNER_STATE_LT):

                if (c == '=') { 
                    token->type = T_LET;
                }

                else { 
                    ungetc( c, srcF ); 
                    token->type = T_LTH;
                }

                ds_free( scannerString );
                return SCAN_OK;

            break;

            case (SCANNER_STATE_TILD):

                if (c == '=') {
                    ds_free( scannerString );
                    token->type = T_NEQ;
                    return SCAN_OK;
                } 

                else {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }

            break;

            case (SCANNER_STATE_EQUAL_SIGN): 
                
                if (c == '=') {
                    ds_free( scannerString );
                    token->type = T_IEQ;
                    return SCAN_OK;
                }

                else {
                    ungetc( c, srcF );
                    ds_free( scannerString );
                    token->type = T_EQU;
                    return SCAN_OK;
                }

            break;

            // COMMENT PROCESSING
            case (SCANNER_STATE_FIRST_DASH): // -

                if (c == '-') {
                    scannerState = SCANNER_STATE_COMMENT_LINE;
                } 
                
                else {
                    ungetc( c, srcF );
                    ds_free( scannerString );
                    token->type = T_SUB;
                    return SCAN_OK;
                }

            break;

            case (SCANNER_STATE_COMMENT_LINE): // --

                if (c == '[') {
                    scannerState = SCANNER_STATE_COMMENT_LSB;
                }

                else if (c == '\n' || c == EOF) {
                    ungetc( c, srcF );
                    scannerState = SCANNER_STATE_START;
                }

                else {
                    scannerState = SCANNER_STATE_COMMENT_READ;
                }

            break;

            case (SCANNER_STATE_COMMENT_LSB): // --[

                if (c == '[') {
                    scannerState = SCANNER_STATE_COMMENTBLOCK;
                }

                else if (c == '\n' || c == EOF) {
                    ungetc( c, srcF );
                    scannerState = SCANNER_STATE_START;
                }

                else {
                    scannerState = SCANNER_STATE_COMMENT_READ;
                }

            break;

            case (SCANNER_STATE_COMMENT_READ): // reading line comment

                if (c == '\n' || c == EOF) {
                    ungetc( c, srcF );
                    scannerState = SCANNER_STATE_START;
                }
            
            break;

            case (SCANNER_STATE_COMMENTBLOCK): // --[[

                if (c == ']') {
                    scannerState = SCANNER_STATE_COMMENTBLOCK_EXIT;
                }

                else if (c == EOF) {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }

            break;

            case (SCANNER_STATE_COMMENTBLOCK_EXIT): // --[[ X ]

                if (c == '[') {
                    scannerState = SCANNER_STATE_START;
                }

                else if (c == EOF) {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }

                else {
                    scannerState = SCANNER_STATE_COMMENTBLOCK;
                }

            break;
            
            // DIV OR IDIV
            case (SCANNER_STATE_SLASH):

                if (c == '/') {
                    token->type = T_IDI;
                }

                else {
                    ungetc( c, srcF );
                    token->type = T_DIV;
                }

                ds_free( scannerString );
                return SCAN_OK;

            break;

            // CONCATENATION
            case (SCANNER_STATE_DOT):

                if (c == '.') {
                    token->type = T_CON;
                    ds_free( scannerString );
                    return SCAN_OK;
                }

                else {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }

            break;

            // STRING PROCESSING
            case (SCANNER_STATE_STRING):

                if (c >= 32) {

                    if (c == '"') {
                        if (!ds_copy( scannerString, token->attribute.string )) {
                            ds_free( scannerString );
                            return ERR_INTERNAL;
                        }
                        token->type = T_STR;
                        ds_free( scannerString );
                        return SCAN_OK;
                    }

                    else if (c == '\\') {
                        scannerState = SCANNER_STATE_ESC_SEQ;
                    }

                    else {
                        if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                        }
                    }
                    
                } else {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }

            break;
            // ESCAPE SEQUENCE
            case (SCANNER_STATE_ESC_SEQ):

                if (c >= 32) {

                    if (c == '"') {
                        c = '"';
                        if (!ds_add_char( scannerString, c )) {
                            ds_free( scannerString );
                            return ERR_INTERNAL;
                        }
                        scannerState = SCANNER_STATE_STRING;
                    }

                    else if (c == 'n') {
                        c = '\n';
                        if (!ds_add_char( scannerString, c )) {
                            ds_free( scannerString );
                            return ERR_INTERNAL;
                        }
                        scannerState = SCANNER_STATE_STRING;
                    }

                    else if (c == 't') {
                        c = '\t';
                        if (!ds_add_char( scannerString, c )) {
                            ds_free( scannerString );
                            return ERR_INTERNAL;
                        }
                        scannerState = SCANNER_STATE_STRING;
                    }

                    else if (c == '\\') {
                        c = '\\';
                        if (!ds_add_char( scannerString, c )) {
                            ds_free( scannerString );
                            return ERR_INTERNAL;
                        }
                        scannerState = SCANNER_STATE_STRING;
                    }

                    else if (c == '0') {
                        ESstr[0] = c;
                        scannerState = SCANNER_STATE_ESC_ZERO;
                    }

                    else if (c == '1') {
                        ESstr[0] = c;
                        scannerState = SCANNER_STATE_ESC_ONE;
                    }

                    else if (c == '2') {
                        ESstr[0] = c;
                        scannerState = SCANNER_STATE_ESC_TWO;
                    }

                } else {
                    ds_free( scannerString );
                    return ERR_INTERNAL;
                }

            break;

            case (SCANNER_STATE_ESC_ZERO):

                if (c == '0') {
                    ESstr[1] = c;
                    scannerState = SCANNER_STATE_ESC_ZERO_ZERO;
                }

                else if (c >= '1' && c <= '9') {
                    ESstr[1] = c;
                    scannerState = SCANNER_STATE_ESC_OTHER;
                }

                else {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }

            break;
            
            case (SCANNER_STATE_ESC_ONE):

                if (c >= '1' && c <= '9') {
                    ESstr[1] = c;
                    scannerState = SCANNER_STATE_ESC_OTHER;
                }

                else {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }

            break;

            case (SCANNER_STATE_ESC_TWO):

                if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4') {
                    ESstr[1] = c;
                    scannerState = SCANNER_STATE_ESC_OTHER;
                }
                
                else if (c == '5') {
                    ESstr[1] = c;
                    scannerState = SCANNER_STATE_ESC_TWO_FIVE;
                }
					
                else {
                    ds_free( scannerString );
					return ERR_LEXICAL;
                }

            break;

            case (SCANNER_STATE_ESC_ZERO_ZERO):

                if (c >= '1' && c <= '9') {
                    ESstr[2] = c;

                    char *ptr;
                    int tmp = (int) strtol( ESstr, &ptr, 10 );
                    c = (char) tmp;
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }

                    scannerState = SCANNER_STATE_STRING;
                }

                else {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }

            break;

            case (SCANNER_STATE_ESC_OTHER):

                if (c >= '0' && c <= '9') {
                    ESstr[2] = c;

                    char *ptr;
                    int tmp = (int) strtol( ESstr, &ptr, 10 );
                    c = (char) tmp;
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }

                    scannerState = SCANNER_STATE_STRING;
                }

                else {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }

            break;

            case (SCANNER_STATE_ESC_TWO_FIVE):

                if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5') {
                    ESstr[2] = c;

                    char *ptr;
                    int tmp = (int) strtol( ESstr, &ptr, 10 );
                    c = (char) tmp;
                    if (!ds_add_char( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }

                    scannerState = SCANNER_STATE_STRING;
                }

                else {
                    ds_free( scannerString );
                    return ERR_LEXICAL;
                }

            break;

        }
    }
}
#endif