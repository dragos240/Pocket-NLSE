//Pocket-NLSE
//Special thanks to qlutoo, yellows8 (for advice and much of the menu functionality), and profi200
#include <stdio.h>

#include <3ds.h>

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
	}

main_cleanup:
	if(opened)
		if((ret = close_archives()))
			error_display_res(NULL, NULL, ret, "Failed to close archives.");
	gfx_fini();

	return 0;
}
