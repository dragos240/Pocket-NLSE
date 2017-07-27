#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "core/conf.h"

//tokens must be array or pre-allocated pointer
int conf_parse(const char* data, conftok_t* token){
	token->mediatype_val = data[0];

	return 0;
}

//data must not be a pointer to an array
int conf_gen(char** data, conftok_t* token){
	*data = calloc(1+1, 1);
	*data[0] = token->mediatype_val;

	return 0;
}
