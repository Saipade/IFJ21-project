/*

*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "string_processor.h"

bool cs_init (Current_string *cs) {

    return true;
}

void cs_clear (Current_string *cs) {
    cs->length = 0;
    cs->str[cs->length] = '\0';
}

bool cs_read_next (Current_string *cs, char c) {

    return true;
}

void cs_mem_free (Current_string *cs) {
    free(cs->str);
}
