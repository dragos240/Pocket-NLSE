#include <stdio.h>
#include <stdlib.h>

#include <3ds.h>

#include "ui/list.h"
#include "ui/ui.h"
#include "core/screen.h"
#include "core/common.h"
#include "core/linkedlist.h"
#include "actions/menus.h"
#include "ui/menu.h"
#include "actions/simple.h"
#include "ui/error.h"

static void grassMenu_open();
void grass_revive();
void grass_strip();

static list_item grassOptions = {"Grass options", COLOR_BLACK, grassMenu_open};
static list_item unburyFossils = {"Unbury fossils", COLOR_BLACK, unbury_fossils};
static list_item grassRevive = {"Revive grass", COLOR_BLACK, grass_revive};
static list_item grassStrip = {"Strip grass", COLOR_BLACK, grass_strip};

void grass_revive(){
	set_grass(gardenData, 0xFF);
}

void grass_strip(){
	set_grass(gardenData, 0x00);
}

static void grassMenu_update(ui_view* view, void* data, linked_list* items, list_item* selected, bool selectedTouched){
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
		linked_list_add(items, &grassStrip);
		linked_list_add(items, &grassRevive);
	}
}

static void grassMenu_open(){
	list_display("Grass Options", "A: Select, B: Go back", NULL, grassMenu_update, genericMenu_draw_top);
}

static void mapMenu_update(ui_view* view, void* data, linked_list* items, list_item* selected, bool selectedTouched){
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
		linked_list_add(items, &grassOptions);
		linked_list_add(items, &unburyFossils);
	}
}

void mapMenu_open(){
	if(!get_loaded_status()){
		error_display(NULL, NULL, "Please dump your save first!");
		return;
	}

	list_display("Map Options", "A: Select, B: Go back", NULL, mapMenu_update, genericMenu_draw_top);
}
