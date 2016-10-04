//Pocket-NLSE
//Special thanks to qlutoo, yellows8 (for advice and much of the menu functionality), and profi200

#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>

#include "log.h"
#include "gfx.h"
#include "menu.h"
#include "dir.h"
#include "ui.h"
#include "kb.h"
#include "dir.h"
#include "menus.h"
#include "tests.h"

//static const u32 second = 1000000000;

u8* gardenData = NULL;

static u32 fontheight = 11;
static u32 debug = 0; //1 - Run tests, 2 = include list_test()

int main(){
	if(debug == 1){
		run_tests();
		return 0;
	}

	int is_loaded = 0;

	int menuindex=0;
	int menucount;

	char headerstr[1024];

	char* menu_entries[] = {
		"Open garden.dat (do this first!)",
		"Map options",
		"Player options",
		"Villager options (nothing here yet!)",
		"Misc options",
		"Save garden.dat (do this last!)",
		"List test"
	};

	gfx_init();

	while(1){
menus_test_loop_start:
		if(debug == 2)
			menucount = 7;
		else
			menucount = 6;

		memset(headerstr, 0, sizeof(headerstr));
		snprintf(headerstr, sizeof(headerstr)-1, "Pocket NLSE");
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1){
			if(!is_loaded)
				break;
			else{
				if(gfx_prompt("Your changes have not been saved. Are you sure you want\nto quit?", NULL)){
					menuindex = 0;
					goto menus_test_loop_start;
				}
				else
					goto menus_test_exit;
			}
		}

		switch(menuindex){
			case 0:
				load_garden();
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
				//villager_select();
				break;
			case 4:
				misc_menu();
				break;
			case 5:
				save_changes();
				if(get_loaded_status() == 0)
					is_loaded = 0;
				break;
			case 6:
				list_test();
				break;
		}
	}

menus_test_exit:

	while(1){
		hidScanInput();

		if(hidKeysDown() & KEY_START)
			break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame();
			sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Press the START button to exit.");
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}

	gfx_fini();
	

	return 0;
}
