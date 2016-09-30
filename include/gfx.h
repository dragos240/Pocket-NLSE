#ifndef NLSE_GFX_H
#define NLSE_GFX_H

#include <sf2d.h>
#include <sftd.h>

extern sf2d_texture* arrow;
extern sftd_font* font;
extern sftd_font* font_bold;

typedef struct{
	int x;
	int y;
} pos_t;

extern pos_t current_pos;

extern const int COLOR_WHITE;
extern const int COLOR_BLACK;

void gfx_init();
void gfx_fini();

void gfx_waitbutton();
int gfx_prompt(char* message, char* keymsg);
int gfx_prompt3(char* message, char* keymsg);

#endif
