#pragma once

#include <string.h>

#include <3ds.h>

#include "constants.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define NA_GAMEID 0x0004000000086300
#define EU_GAMEID 0x0004000000086400
#define JP_GAMEID 0x0004000000086200

#define SECOND_IN_NS (1000*1000*1000)

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLACK 0xFF000000
#define COLOR_TEXT 0xFF000000

extern u32 debug;
extern bool devmode;

extern u8* gardenData;

char* u16str2str(u16* wstr);
