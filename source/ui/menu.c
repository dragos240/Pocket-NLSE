#include <stdio.h>
#include <stdlib.h>

#include <3ds.h>

#include "ui/ui.h"
#include "ui/list.h"
#include "core/screen.h"

void genericMenu_draw_top(ui_view* view, void* data, float x1, float y1, float x2, float y2, list_item* selected){
	u32 logoWidth;
	u32 logoHeight;

	//Warnings warnings go away
	view = view;
	data = data;
	selected = selected;

	screen_get_texture_size(&logoWidth, &logoHeight, TEXTURE_LOGO);

	float logoX = x1 + (x2 - x1 - logoWidth)/2;
	float logoY = y1 + (y2 - y1 - logoHeight)/2;
	screen_draw_texture(TEXTURE_LOGO, logoX, logoY, logoWidth, logoHeight);
}
