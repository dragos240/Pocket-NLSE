//Pocket-NLSE
//Special thanks to qlutoo, yellows8 (for advice and much of the menu functionality), and profi200
#include <stdio.h>

#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>

#include "common.h"
#include "gfx.h"
#include "menu.h"
#include "dir.h"
#include "ui.h"
#include "kb.h"
#include "dir.h"
#include "actions.h"
#include "menus.h"
#include "fs.h"
#include "backup.h"
#include "tests.h"

u8* gardenData = NULL;
bool items_initialized = false;

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
	bool opened = false;

	int menuindex=0;
	int menucount;

	char headerstr[1024];
	char* version = "v1.0.0";

	char* menu_entries[] = {
		"Load/restore (do this first!)",
		"Map options",
		"Player options",
		"Villager options",
		"Misc options",
		"Inject changes (do this last!)",
		"List test"
	};

	gfx_init();
	if(is3dsx){
		ret = fs_init();
		if(ret)
			gfx_error(ret, __FILENAME__, __LINE__);
	}
	if(get_mediatype())
		goto main_cleanup;
	if(get_titleid() == -1)
		goto main_cleanup;
	if((ret = open_archives())){
		gfx_error(ret, __FILENAME__, __LINE__);
		goto main_cleanup;
	}
	opened = true;

	memset(headerstr, 0, sizeof(headerstr));
	snprintf(headerstr, sizeof(headerstr)-1, "Pocket NLSE - %s", version);
	while(aptMainLoop()){
		if(devmode == true)
			menucount = 8;
		else
			menucount = 6;

		if(is_loaded == 1 && items_initialized == false){
			items_init(gardenData);
			items_initialized = true;
		}

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
				inject_changes();
				if(get_loaded_status() == 0)
					is_loaded = 0;
				break;
			case 6:
				list_test();
				break;
		}
	}

main_cleanup:
	if(opened)
		if((ret = close_archives()))
			gfx_error(ret, __FILENAME__, __LINE__);
	if(is3dsx){
		fs_fini();
	}
	gfx_fini();

	return 0;
}
