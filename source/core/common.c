#include <stdlib.h>

#include <3ds.h>

char* u16str2str(u16* wstr){
	int len = 0;
	char* str;

	str = malloc(1);
	len++;
	while(wstr[len-1] < 128){
		str[len-1] = (char)(wstr[len-1]);
		len++;
		str = realloc(str, len);
	}

	return str;
}
