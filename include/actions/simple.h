#pragma once

#include <stdbool.h>

#include <3ds.h>

#include "core/constants.h"

extern u8 data_offset;

typedef struct{
	u16 itemid;
	u8 flag1;
	u8 flag2;
} item_t;

extern item_t map_items[SIZE_MAP_ITEMS];
extern item_t pockets[4][SIZE_PLAYER_POCKETS];
extern item_t dressers[4][SIZE_PLAYER_DRESSERS];
extern item_t islandbox[4][SIZE_PLAYER_ISLANDBOX];
extern item_t storage[4][SIZE_PLAYER_STORAGE];

void items_init(u8* data);

u8 readByte1(u8* data, u32 pos);
u16 readByte2(u8* data, u32 pos);
u32 readByte4(u8* data, u32 pos);
u32* readBytes(u8* data, u32 pos, u8 nbytes);

void storeByte(u8* dst_data, u32 pos, u8 src_data);
void storeByte2(u8* dst_data, u32 pos, u16 src_data);
void storeByte4(u8* dst_data, u32 pos, u32 src_data);

u8* get_ustring(u8* data, u32 offset, u32 max_length);
void set_ustring(u8* data, u32 offset, u8* str, u32 max_length);

void set_grass(u8* data, u8 ratio);

u32 gen_crc32(u8* bytes, s32 length);
void updateChecksum(u8* data, u32 offset, u32 length);
void writeChecksums(u8* data);
<<<<<<< Updated upstream:include/actions.h

#endif
=======
>>>>>>> Stashed changes:include/actions/simple.h
