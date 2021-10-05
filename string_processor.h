/*

*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
	char *str; 
	int length; 
	int size; // for mem. allocation
} Current_string;

bool cs_init (Current_string *str); // initialization
void cs_clear (Current_string *str); // removal 
bool cs_read_next (Current_string *str, char s); // add next char
void cs_mem_free (Current_string *cs); // memory free