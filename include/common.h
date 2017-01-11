#ifndef NLSE_COMMON_H
#define NLSE_COMMON_H

#include <3ds.h>

#include "constants.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define NA_GAMEID 0x0004000000086300
#define EU_GAMEID 0x0004000000086400
#define JP_GAMEID 0x0004000000086200

#define SECOND_IN_NS (1000*1000*1000)

extern u32 debug;
extern bool devmode;
extern bool is3dsx;

extern u8* gardenData;

void setIs3dsx();

char* u16str2str(u16* wstr);

#endif
