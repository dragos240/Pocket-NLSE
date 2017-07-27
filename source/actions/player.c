#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>

#include "ui/list.h"
#include "ui/ui.h"
#include "core/linkedlist.h"
#include "actions/simple.h"
#include "core/constants.h"
#include "core/common.h"
#include "ui/error.h"
#include "ui/menu.h"
#include "actions/menus.h"
#include "ui/list.h"
#include "ui/prompt.h"
#include "actions/player/bank.h"
#include "actions/player/tan.h"
#include "actions/player/hairstyle.h"
#include "actions/player/haircolor.h"
#include "actions/player/eyecolor.h"
#include "actions/player/facetype.h"
#include "actions/player.h"

static char** playerNames;
static char* selectedPlayerName;
static player_t* playerData;

static list_item* playerSelectOptions;
static list_item playerOption;

void tanMenu_open(list_item* selected);
void hairstyleMenu_open(list_item* selected);
void haircolorMenu_open(list_item* selected);
void eyecolorMenu_open(list_item* selected);
void facetypeMenu_open(list_item* selected);

static list_item bankMenu = {"Bank options", COLOR_BLACK, bankMenu_open};
static list_item tanMenu = {"Change tan shade", COLOR_BLACK, tanMenu_open};
static list_item hairstyleMenu = {"Change hairstyle", COLOR_BLACK, hairstyleMenu_open};
static list_item haircolorMenu = {"Change hair color", COLOR_BLACK, haircolorMenu_open};
static list_item eyecolorMenu = {"Change eye color", COLOR_BLACK, eyecolorMenu_open};
static list_item facetypeMenu = {"Change face type", COLOR_BLACK, facetypeMenu_open};

void tanMenu_open(list_item* selected){
	//list_display("Tan Menu", "A: Select, B: Go back", selected, tanMenu_update, genericMenu_draw_top);
}

void hairstyleMenu_open(list_item* selected){
}

void haircolorMenu_open(list_item* selected){
}

void eyecolorMenu_open(list_item* selected){
}

void facetypeMenu_open(list_item* selected){
}


static void playerMenu_update(ui_view* view, void* data, linked_list* items, list_item* selected, bool selectedTouched){
	list_item* actionData = (list_item*)data;

	if(hidKeysDown() & KEY_B){
		ui_pop();
		list_destroy(view);

		return;
	}

	if(selected != NULL && (selectedTouched || hidKeysDown() & KEY_A) && selected->data != NULL){
		void (*action)(list_item*) = (void(*)(list_item*))selected->data;
		action(actionData);
		return;
	}

	if(linked_list_size(items) == 0){
		linked_list_add(items, &bankMenu);
		linked_list_add(items, &tanMenu);
		linked_list_add(items, &hairstyleMenu);
		linked_list_add(items, &haircolorMenu);
		linked_list_add(items, &eyecolorMenu);
		linked_list_add(items, &facetypeMenu);
	}
}

static void playerMenu_open(list_item* selected){
	//prompt_display("Info", selected->name, COLOR_BLACK, false, NULL, NULL, NULL);

	list_display("Player Options", "A: Select, B: Go back", selected, playerMenu_update, genericMenu_draw_top);
}

static void playerSelectMenu_update(ui_view* view, void* data, linked_list* items, list_item* selected, bool selectedTouched){
	//Warnings warnings go away
	data = data;

	if(hidKeysDown() & KEY_B){
		ui_pop();
		list_destroy(view);

		return;
	}

	if(linked_list_size(items) == 0){
		playerNames = malloc(4*sizeof(char*));
		int i;
		playerSelectOptions = malloc(4*sizeof(list_item));
		playerData = malloc(4*sizeof(player_t));
		playerData[0].name = playerData[1].name = playerData[2].name = playerData[3].name = NULL;
		for(i = 0; i < 4; i++){
			playerNames[i] = (char*)get_ustring(gardenData, (OFFSET_PLAYERS+SIZE_PLAYER*i)+OFFSET_PLAYER_NAME, 10);
			if(strlen(playerNames[i]) == 0){
				break;
			}
			playerData[i].name = playerNames[i];
			playerData[i].offset = (OFFSET_PLAYERS+SIZE_PLAYER*i)+OFFSET_PLAYER_NAME;
			sprintf(playerSelectOptions[i].name, playerNames[i]);
			playerSelectOptions[i].color = COLOR_BLACK;
			playerSelectOptions[i].data = &playerData[i];
			linked_list_add(items, &playerSelectOptions[i]);
		}
	}

	//if(selected != NULL && (selectedTouched || hidKeysDown() & KEY_A) && selected->data != NULL){
	if(selected != NULL && (selectedTouched || hidKeysDown() & KEY_A)){
		playerMenu_open(selected);
		return;
	}
}

void playerSelectMenu_open(){
	if(!get_loaded_status()){
		error_display(NULL, NULL, "Please dump your save first!");
		return;
	}

	list_display("Player Select", "A: Select, B: Go back", NULL, playerSelectMenu_update, genericMenu_draw_top);
}
