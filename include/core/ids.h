#pragma once

#include <3ds.h>

typedef struct{
	char name[10];
	u16 type; //personality
	u8 catchphrase[15];
	u32 data[22];
} villager_t;

typedef struct{
	u8 r; //red
	u8 g; //green
	u8 b; //blue
} color_t;

extern villager_t villagers[399];
extern color_t colors[19];
