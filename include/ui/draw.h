#pragma once

#include <3ds.h>

typedef struct{
	u8* tile_pos; //[x, y]
	u8* acre_pos; //[x, y]
	u16 tile_item;
	u8 flag1;
	u8 flag2;
	u8 acre_type;
} mapctrl_t;

void draw_pockets(int player);
void draw_map(mapctrl_t* mapctrl, u8* acres);
