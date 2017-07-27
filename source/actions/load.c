#include <stdio.h>
#include <stdlib.h>

#include <3ds.h>

#include "ui/list.h"
#include "ui/ui.h"
#include "core/screen.h"
#include "core/common.h"
#include "core/linkedlist.h"
#include "actions/menus.h"

static list_item dumpAndBackup = {"Dump save & backup", COLOR_BLACK, dump_and_backup_garden};
static list_item restoreBackup = {"Restore backup", COLOR_BLACK, restore_backup};

static void load_draw_top(ui_view* view, void* data, float x1, float y1, float x2, float y2, list_item* selected){
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

static void load_update(ui_view* view, void* data, linked_list* items, list_item* selected, bool selectedTouched){
	//Warnings warnings go away
	data = data;

	if(hidKeysDown() & KEY_B){
		ui_pop();
		list_destroy(view);

		return;
	}

	if(selected != NULL && (selectedTouched || hidKeysDown() & KEY_A) && selected->data != NULL){
		((void(*)()) selected->data)();
		return;
	}

	if(linked_list_size(items) == 0){
		linked_list_add(items, &dumpAndBackup);
		linked_list_add(items, &restoreBackup);
	}
}

void load_open(){
	list_display("Load/restore", "A: Select, B: Go back", NULL, load_update, load_draw_top);
}
