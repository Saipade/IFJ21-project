/**
 * PROJECT: Implementation of imperative programming language compiler
 * PART: Dynamic string
 *
 * AUTHOR(S): Maksim Tikhonov (xtikho00) 
 */

#ifndef STRING_PROCESSOR_H
#define STRING_PROCESSOR_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// For dynamic string memory allocation
#define DYNAMIC_STR_STARTING_MEM 20
#define DYNAMIC_STR_INCREASE_MEM 10

// String processing structure
typedef struct {

	char *str;              // string itself
	int length;             // length of string
    int size;               // for memory allocation

} Dynamic_string;

// String processing

/** 
 * Initializes dynamic string
 * @param str pointer to current dynamic string
 * @return
 */
bool ds_init ( Dynamic_string *str );

/** 
 * Adds character to the end of dynamic string
 * @param str pointer to dynamic string
 * @param c character that will be added
 * @return
 */
bool ds_add_char ( Dynamic_string *str, char c );

/** 
 * Copies string from one dynamic string to another 
 * @param src pointer to source dynamic string
 * @param dst pointer to destination dynamic string
 * @return
 */
bool ds_copy ( Dynamic_string *src, Dynamic_string *dst );

/**
 * Concatenes dynamic string and constant string
 * @param dynamicStr pointer to dynamic string
 * @param str constant dynamic string
 * @return true in case if success, false - otherwise
 */ 
bool ds_add_chars ( Dynamic_string *dynamicStr, char *str);

/** 
 * Frees allocated memory of dynamic string
 * @param cStr pointer to current dynamic string
 * @return
 */
void ds_free ( Dynamic_string *str );

#endif