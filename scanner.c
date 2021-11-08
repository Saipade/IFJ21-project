/*

*/
#ifndef SCANNER_C
#define SCANNER_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scanner.h"
#include "errorslist.h"

// Pointer to file that will be scanned
FILE *srcF;

// Dynamic string for token               
Dynamic_string *dynamicString;

void _dynamic_string ( Dynamic_string *string ) {

	dynamicString = string;

}

int get_next_token ( Token *token ) {

    if (!srcF || !dynamicString) {
        return ERR_INTERNAL;
    }
    int scannerState = SCANNER_STATE_START;
    
    token->type = TT_NDA;
    token->attribute.string = dynamicString;

    Dynamic_string str;
    Dynamic_string *scannerString = &str;
    if (!ds_init( scannerString )) return ERR_INTERNAL;
    
    char ESstr[3], c;
    
    while (true) {

        c = (char) getc( srcF );

        switch (scannerState) {

            case (SCANNER_STATE_START):

                if (c == ' ' || c == '\n') {
                    scannerState = SCANNER_STATE_START;
                }

                else if (c >= '1' && c <= '9') {
                    if (!ds_add_next( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                    scannerState = SCANNER_STATE_INT;
                }

                else if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                    if (!ds_add_next( scannerString, c )) {
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
                    token->type = TT_ADD;
                    return SCAN_OK;
                } 

                else if (c == '-') {
                    scannerState = SCANNER_STATE_FIRST_DASH;
                } 

                else if (c == '*') { 
                    ds_free( scannerString );
                    token->type = TT_MUL;
                    return SCAN_OK;
                } 

                else if (c == '/') { 
                    scannerState = SCANNER_STATE_SLASH;

                } 

                else if (c == '=') { 
                    scannerState = SCANNER_STATE_EQUAL_SIGN;
                    ds_free( scannerString );
                    token->type = TT_EQU;
                    return SCAN_OK;
                }

                else if (c == '~') {
                    scannerState = SCANNER_STATE_TILD;
                }
                
                else if (c == ':') { 
                    ds_free( scannerString );
                    token->type = TT_COL;
                    return SCAN_OK;
                }

                else if (c == ',') { 
                    ds_free( scannerString );
                    token->type = TT_COM;
                    return SCAN_OK;
                }
                
                else if (c == '(') { 
                    ds_free( scannerString );
                    token->type = TT_LBR;
                    return SCAN_OK;
                }
                
                else if (c == ')') { 
                    ds_free( scannerString );
                    token->type = TT_RBR;
                    return SCAN_OK;
                }
                
                else if (c == '#') { 
                    ds_free( scannerString );
                    token->type = TT_LEN;
                }

                else if (c == '.') {
                    scannerState = SCANNER_STATE_DOT;
                }
                
                else if (c == '"') {
                    scannerState = SCANNER_STATE_STRING;
                }

                else if (c == EOF) { 
                    ds_free( scannerString );
                    token->type = TT_EOF;
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
                    if (!ds_add_next( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                } 
                
                else if (c == '.') {
                    scannerState = SCANNER_STATE_POINT;
                    if (!ds_add_next( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                } 
                
                else if (c == 'E' || c == 'e') {
                    scannerState = SCANNER_STATE_EXP;
                    if (!ds_add_next( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                }

                else { 
                    ungetc( c, srcF );
                    token->type = TT_INT;
                    return _integer_or_number( scannerString, token );
                }

            break;
            
            case (SCANNER_STATE_POINT):

                if (c >= '0' && c <= '9') {
                    scannerState = SCANNER_STATE_DOUBLE;
                    if (!ds_add_next( scannerString, c )) {
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
                    if (!ds_add_next( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                } 

                else if (c == 'e' && c == 'E') {
                    scannerState = SCANNER_STATE_EXP;
                    if (!ds_add_next( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                }

                else { 
                    ungetc( c, srcF );
                    token->type = TT_DOU;
                    return _integer_or_number( scannerString, token );
                }

            break;

            case (SCANNER_STATE_EXP):

                if (c >= '0' && c <= '9') {
                    scannerState = SCANNER_STATE_EXP_NUM;
                    if (!ds_add_next( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                }

                else if (c == '+' || c == '-') {
                    scannerState = SCANNER_STATE_EXP_SIGN;
                    if (!ds_add_next( scannerString, c )) {
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
                    if (!ds_add_next( scannerString, c )) {
                        ds_free( scannerString );
                        return ERR_INTERNAL;
                    }
                }

                else {
                    ungetc( c, srcF );
                    token->type = TT_DOU;
                    return _integer_or_number( scannerString, token );
                }
                
            break;

            case (SCANNER_STATE_EXP_SIGN):

                if(c >= '0' && c <= '9') {
                    scannerState = SCANNER_STATE_EXP_NUM;
                    if (!ds_add_next( scannerString, c )) {
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
                    if (!ds_add_next( scannerString, c )) {
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
                    token->type = TT_MET;
                }

                else { 
                    ungetc( c, srcF );
                    token->type = TT_MTH;
                    
                }

                ds_free( scannerString );
                return SCAN_OK;

            break;

            case (SCANNER_STATE_LT):

                if (c == '=') { 
                    token->type = TT_LET;
                }

                else { 
                    ungetc( c, srcF ); 
                    token->type = TT_LTH;
                }

                ds_free( scannerString );
                return SCAN_OK;

            break;

            case (SCANNER_STATE_TILD):

                if (c == '=') {
                    ds_free( scannerString );
                    token->type = TT_NEQ;
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
                    token->type = TT_IEQ;
                    return SCAN_OK;
                }

                else {
                    ungetc( c, srcF );
                    ds_free( scannerString );
                    token->type = TT_EQU;
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
                    token->type = TT_SUB;
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
                    token->type = TT_IDI;
                }

                else {
                    ungetc( c, srcF );
                    token->type = TT_DIV;
                }

                ds_free( scannerString );
                return SCAN_OK;

            break;

            // CONCATENATION
            case (SCANNER_STATE_DOT):

                if (c == '.') {
                    token->type = TT_CON;
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
                        token->type = TT_STR;
                        ds_free( scannerString );
                        return SCAN_OK;
                    }

                    else if (c == '\\') {
                        scannerState = SCANNER_STATE_ESC_SEQ;
                    }

                    else {
                        if (!ds_add_next( scannerString, c )) {
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
                        if (!ds_add_next( scannerString, c )) {
                            ds_free( scannerString );
                            return ERR_INTERNAL;
                        }
                        scannerState = SCANNER_STATE_STRING;
                    }

                    else if (c == 'n') {
                        c = '\n';
                        if (!ds_add_next( scannerString, c )) {
                            ds_free( scannerString );
                            return ERR_INTERNAL;
                        }
                        scannerState = SCANNER_STATE_STRING;
                    }

                    else if (c == 't') {
                        c = '\t';
                        if (!ds_add_next( scannerString, c )) {
                            ds_free( scannerString );
                            return ERR_INTERNAL;
                        }
                        scannerState = SCANNER_STATE_STRING;
                    }

                    else if (c == '\\') {
                        c = '\\';
                        if (!ds_add_next( scannerString, c )) {
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
                    if (!ds_add_next( scannerString, c )) {
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
                    if (!ds_add_next( scannerString, c )) {
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
                    if (!ds_add_next( scannerString, c )) {
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