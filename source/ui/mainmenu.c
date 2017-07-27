#include <stdio.h>
#include <stdlib.h>

#include <3ds.h>

#include "ui/list.h"
#include "core/fs.h"
#include "actions/load.h"
#include "actions/map.h"
#include "actions/player.h"
#include "ui/ui.h"
#include "core/screen.h"
#include "core/common.h"
#include "core/linkedlist.h"
#include "actions/menus.h"
#include "ui/menu.h"

static list_item loadRestore = {"Load/restore (do this first!)", COLOR_BLACK, load_open};
static list_item mapOptions = {"Map options", COLOR_BLACK, mapMenu_open};
static list_item playerOptions = {"Player options", COLOR_BLACK, playerSelectMenu_open};
static list_item villagerOptions = {"Villager options", COLOR_BLACK, villager_select};
static list_item miscOptions = {"Misc options", COLOR_BLACK, misc_menu};
static list_item injectChanges = {"Inject changes (do this last!)", COLOR_BLACK, inject_changes};
static list_item changeMediaSource = {"Change media source", COLOR_BLACK, change_mediatype};

static void mainmenu_update(ui_view* view, void* data, linked_list* items, list_item* selected, bool selectedTouched){
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
		linked_list_add(items, &loadRestore);
		linked_list_add(items, &mapOptions);
		linked_list_add(items, &playerOptions);
		linked_list_add(items, &villagerOptions);
		linked_list_add(items, &miscOptions);
		linked_list_add(items, &injectChanges);
		linked_list_add(items, &changeMediaSource);
	}
}

void mainmenu_open(){
	list_display("Main Menu", "A: Select, B: Exit", NULL, mainmenu_update, genericMenu_draw_top);
}
