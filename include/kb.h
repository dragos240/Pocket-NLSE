#ifndef NLSE_KB_H
#define NLSE_KB_H

typedef struct{
	int start_x;
	int end_x;
	int start_y;
	int end_y;
	char* contents;
} kbkey_t;

void kb_init();
void kb_fini();
char* draw_kb(char* str);

#endif
