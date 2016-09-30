#include <stdio.h>
#include <stdarg.h>

#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>

#include "log.h"
#include "ui.h"
#include "gfx.h"

sf2d_texture* arrow;
sftd_font* font;
sftd_font* font_bold;

static int fontheight = 11;
//static int fontwidth = 7;

const int COLOR_WHITE = RGBA8(0xFF, 0xFF, 0xFF, 0xFF);
const int COLOR_BLACK = RGBA8(0x00, 0x00, 0x00, 0xFF);

pos_t current_pos;

void gfx_init(){
	sf2d_init();
	romfsInit();
	sftd_init();
	ui_init();
	arrow = sfil_load_PNG_file("romfs:/arrow.png", SF2D_PLACE_RAM);
	font = sftd_load_font_file("romfs:/VeraMono.ttf");
	font_bold = sftd_load_font_file("romfs:/VeraMono-Bold.ttf");
}

void gfx_fini(){
	sftd_free_font(font);
	sf2d_free_texture(arrow);
	ui_fini();
	sftd_fini();
	romfsExit();
	sf2d_fini();
}

void gfx_waitbutton(){
	while(1){
		hidScanInput();

		if(hidKeysDown() & KEY_A)
			break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame();
			sftd_draw_textf(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Press the A button to continue.\n");
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
}

int gfx_prompt(char* message, char* keymsg){
	if(keymsg==NULL)keymsg = "A = Yes, B = No.";

	while(1){
		hidScanInput();

		if(hidKeysDown() & KEY_A)
			return 0;
		else if(hidKeysDown() & KEY_B)
			return 1;
	
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame();
			sftd_draw_textf(font, 0, fontheight*2, RGBA8(0xFF, 0xFF, 0xFF, 0xFF), fontheight, "%s", message);
			sftd_draw_textf(font, 0, fontheight*4, RGBA8(0xFF, 0xFF, 0xFF, 0xFF), fontheight, "%s", keymsg);
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}

	return 0;
}

int gfx_prompt3(char* message, char* keymsg){
	while(1){
		hidScanInput();

		if(hidKeysDown() & KEY_A)
			return 0;
		else if(hidKeysDown() & KEY_B)
			return 1;
		else if(hidKeysDown() & KEY_Y)
			return 2;
	
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame();
			sftd_draw_textf(font, 0, fontheight*2, RGBA8(0xFF, 0xFF, 0xFF, 0xFF), fontheight, "%s", message);
			sftd_draw_textf(font, 0, fontheight*4, RGBA8(0xFF, 0xFF, 0xFF, 0xFF), fontheight, "%s", keymsg);
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}

	return 0;
}
