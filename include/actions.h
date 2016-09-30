#ifndef NLSE_ACTIONS_H
#define NLSE_ACTIONS_H

#include <stdbool.h>

#include <3ds.h>

extern u8 data_offset;

typedef struct{	
	u8 town_id1;
	u8 town_id2;

	u8 grass_type;
	u8* town_name;

	u32* town_id_refs;
} town_t;

void init_garden(u8* data, town_t* town);
void init_town(u8* data, town_t* town);
void save_town(u8* data, town_t* town);

u8 readByte1(u8* data, u32 pos);
u16 readByte2(u8* data, u32 pos);
u32 readByte4(u8* data, u32 pos);
u32* readBytes(u8* data, u32 pos, u8 nbytes);

void storeByte(u8* dst_data, u32 pos, u8 src_data);
void storeByte2(u8* dst_data, u32 pos, u16 src_data);

u8* get_ustring(u8* data, u32 offset, u32 max_length);
void set_ustring(u8* data, u32 offset, u8* str, u32 max_length);

void set_grass(u8* data, u8 ratio);

u32 gen_crc32(u8* bytes, s32 length);
void updateChecksum(u8* data, u32 offset, u32 length);

#endif
