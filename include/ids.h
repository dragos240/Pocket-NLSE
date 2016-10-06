#ifndef NLSE_IDS_H
#define NLSE_IDS_H

#include <3ds.h>

typedef struct{
	int id;
	int type; //personality
	char name[10];
	char catchphrase[15];
	u32 data[25];
} villager_t;

extern villager_t villagers[333];

#endif
