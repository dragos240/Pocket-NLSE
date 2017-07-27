#include <stdio.h>
#include <stdarg.h>

#include <3ds.h>

#include "ui/gfx.h"
#include "core/screen.h"
#include "ui/ui.h"

pos_t current_pos;

void gfx_init(){
	romfsInit();
	gfxInitDefault();
	screen_init();
	ui_init();
}

void gfx_fini(){
	ui_exit();
	screen_exit();
	gfxExit();
	romfsExit();
}
