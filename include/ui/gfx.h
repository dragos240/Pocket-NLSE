#pragma once

#include <3ds.h>

typedef struct{
	int x;
	int y;
} pos_t;

extern pos_t current_pos;

void gfx_init();
void gfx_fini();
