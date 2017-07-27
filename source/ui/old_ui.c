#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <3ds.h>

#include "core/screen.h"
#include "core/common.h"

void ui_top_frame(char* titletext, ...){
	char buffer[256];
	va_list args;
	memset(buffer, '\0', 256);
	va_start(args, titletext);
	vsnprintf(buffer, 256, titletext, args);
	screen_draw_texture(TEXTURE_TOP_SCREEN_BG, 0, 0, TOP_SCREEN_WIDTH, TOP_SCREEN_HEIGHT);
	screen_draw_string(buffer, 1, 1, 0.6, 0.5, COLOR_BLACK, false);
	screen_draw_string(buffer, 0, 0, 0.6, 0.5, COLOR_WHITE, false);
	va_end(args);
}

void ui_bottom_frame(char* titletext, ...){
	char buffer[256];
	va_list args;
	memset(buffer, '\0', 256);
	va_start(args, titletext);
	vsnprintf(buffer, 256, titletext, args);
	screen_draw_texture(TEXTURE_BOTTOM_SCREEN_BG, 0, 0, BOTTOM_SCREEN_WIDTH, BOTTOM_SCREEN_HEIGHT);
	screen_draw_string(buffer, 1, 1, 0.6, 0.5, COLOR_BLACK, false);
	screen_draw_string(buffer, 0, 0, 0.6, 0.5, COLOR_WHITE, false);
	va_end(args);
}
