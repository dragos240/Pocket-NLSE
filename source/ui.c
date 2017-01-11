#include <stdio.h>
#include <stdarg.h>

#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>

#include "gfx.h"
#include "ui.h"

static int fontheight = 11;

sf2d_texture* topbar;
sf2d_texture* leaf;

void ui_init(){
	sf2d_set_clear_color(RGBA8(0x00, 0x2e, 0x00, 0xFF));

	topbar = sfil_load_PNG_file("romfs:/topbar.png", SF2D_PLACE_RAM);
	leaf = sfil_load_PNG_file("romfs:/leaf.png", SF2D_PLACE_RAM);
}

void ui_fini(){
	sf2d_free_texture(leaf);
	sf2d_free_texture(topbar);
}

void ui_frame(char* titletext, ...){
	char buffer[256];
	va_list args;
	va_start(args, titletext);
	vsnprintf(buffer, 256, titletext, args);
	sf2d_draw_texture(topbar, 0, 0);
	sf2d_draw_texture(leaf, 287, 22);
	sftd_draw_text(font_bold, 1, 1, COLOR_BLACK, fontheight, buffer);
	sftd_draw_text(font_bold, 0, 0, COLOR_WHITE, fontheight, buffer);
	va_end(args);
}
