#include <stdlib.h>
#include <string.h>

struct Dynamic_string {
	char *str; 
	int length; 
	int size; // allocation size
};
