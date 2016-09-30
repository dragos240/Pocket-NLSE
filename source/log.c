#include <stdio.h>

#include "log.h"

FILE* flog = NULL;

void log_line(char* file, int line){
	char* path = "/Pocket-NLSE.log";

	if(flog != NULL)
		fclose(flog);

	flog = fopen(path, "a");

	fprintf(flog, "File: %s: %d\n", file, line);
	
	fclose(flog);
	flog = fopen(path, "a");
}
