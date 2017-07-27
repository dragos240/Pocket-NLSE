//Pocket-NLSE
//Special thanks to qlutoo, yellows8 (for advice and much of the menu functionality), and profi200
#include <stdio.h>

#include <3ds.h>

<<<<<<< Updated upstream
#include "common.h"
#include "log.h"
#include "gfx.h"
#include "menu.h"
#include "dir.h"
#include "ui.h"
#include "kb.h"
#include "dir.h"
#include "menus.h"
#include "backup.h"
#include "tests.h"

//static const u32 second = 1000000000;

u8* gardenData = NULL;

static u32 fontheight = 11;
u32 debug = 0; //1 = Run tests
bool devmode = false;

int main(){
	Result ret;

	setIs3dsx();
	if(debug == 1){
		run_tests();
		return 0;
	}

	hidScanInput();
	if((hidKeysHeld() & KEY_L) && (hidKeysHeld() & KEY_R))
		devmode = true;

	int is_loaded = 0;

	int menuindex=0;
	int menucount;

	char headerstr[1024];

	char* menu_entries[] = {
		"Load/restore options (do this first!)",
		"Map options",
		"Player options",
		"Villager options",
		"Misc options",
		"Save/inject options (do this last!)",
		"List test",
		"View APT ID"
	};

	gfx_init();
	ret = backup_init();
	if(ret){
		gfx_error(ret, __LINE__);
=======
#include "ui/gfx.h"
#include "core/fs.h"
#include "ui/error.h"
#include "ui/mainmenu.h"
#include "ui/ui.h"
#include "actions/simple.h"
#include "actions/menus.h"

u8* gardenData = NULL;
bool items_initialized = false;
bool devmode;

int main(){
	Result ret;
	bool opened = false;

	hidScanInput();
	if(hidKeysHeld() & KEY_R){
		devmode = true;
		return 0;
	}

	gfx_init();
	if((ret = open_sdmc_archive())){
		error_display_res(NULL, NULL, ret, "Failed to open SDMC archive.");
		goto main_cleanup;
	}
	load_config();
	if(get_titleid() == -1)
		goto main_cleanup;
	if((ret = open_game_archive())){
		error_display_res(NULL, NULL, ret, "Failed to open game archive.");
		goto main_cleanup;
	}
	opened = true;

	mainmenu_open();
	while(aptMainLoop() && ui_update()){
		if(get_loaded_status() && items_initialized == false){
			items_init(gardenData);
			items_initialized = true;
		}
>>>>>>> Stashed changes
	}

	/*
	while(aptMainLoop()){
<<<<<<< Updated upstream
menus_test_loop_start:
		if(devmode == true)
			menucount = 8;
		else
			menucount = 6;
=======
		if(is_loaded == 1 && items_initialized == false){
			items_init(gardenData);
			items_initialized = true;
		}
>>>>>>> Stashed changes

		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1){
			if(!is_loaded)
				break;
			else{
				if(gfx_prompt("Your changes have not been saved. Are you sure you want to quit?", NULL)){
					menuindex = 0;
					continue;
				}
				else
					goto main_cleanup;
			}
		}

		switch(menuindex){
			case 0:
				load_menu();
				if(get_loaded_status() == 1)
					is_loaded = 1;
				break;
			case 1:
				map_menu();
				break;
			case 2:
				player_select();
				break;
			case 3:
				villager_select();
				break;
			case 4:
				misc_menu();
				break;
			case 5:
<<<<<<< Updated upstream
				save_menu();
=======
				inject_changes();
>>>>>>> Stashed changes
				if(get_loaded_status() == 0)
					is_loaded = 0;
				break;
			case 6:
				change_mediatype();
				break;
		}
	}*/

<<<<<<< Updated upstream
	ret = backup_fini();
	if(ret)
		gfx_error(ret, __LINE__);
=======
main_cleanup:
	if(opened)
		if((ret = close_archives()))
			error_display_res(NULL, NULL, ret, "Failed to close archives.");
>>>>>>> Stashed changes
	gfx_fini();

	return 0;
}
