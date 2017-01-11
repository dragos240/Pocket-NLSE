#include <locale.h>

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
#include "menus.h"
#include "fs.h"
#include "backup.h"
#include "tests.h"

static int fontheight = 11;

void run_tests(){
	backup_test();
	//mintest();
	//menu_test();
	//ui_test();
	//kb_test();
	//dir_test();
}

void backup_test(){
	Result ret;

	gfx_init();
	get_mediatype();
	if(get_titleid() == -1)
		goto backup_test_cleanup;

	gfx_displaymessage("Backing up cartridge...");

	if((ret = fs_init()))
		gfx_error(ret, __FILENAME__, __LINE__);
	if((ret = open_archives()))
		gfx_error(ret, __FILENAME__, __LINE__);

backup_test_cleanup:
	fs_fini();
	if((ret = close_archives()))
		gfx_error(ret, __FILENAME__, __LINE__);

	gfx_waitmessage("Done! (hopefully)");

	gfx_fini();
}

void dir_test(){
	char* path;

	gfx_init();

	path = browse_dir("Load garden.dat");

	while(aptMainLoop()){
		hidScanInput();
		if(hidKeysDown() & KEY_A)
			break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		ui_frame("Path: %s", path);

		sftd_draw_textf(font, 0, 2*fontheight, COLOR_WHITE, fontheight, "Press the A button to continue.\n");
		sf2d_end_frame();
		if(is3dsx){
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_end_frame();
		}
		sf2d_swapbuffers();
	}

	gfx_fini();
}

void kb_test(){
	gfx_init();

	char* str = get_string("Type in some text");

	while(aptMainLoop()){
		hidScanInput();

		if(hidKeysDown() & KEY_A)
			break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame("Keyboard test");
			sftd_draw_textf(font, 0, fontheight*2, COLOR_WHITE, fontheight, "You entered: %s", str);
			sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
		sf2d_end_frame();
		if(is3dsx){
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_end_frame();
		}
		sf2d_swapbuffers();
	}

	gfx_fini();
}

void ui_test(){	
	gfx_init();

	while(aptMainLoop()){
		hidScanInput();

		if(hidKeysDown() & KEY_START)
			break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame("UI test");
		sf2d_end_frame();
		if(is3dsx){
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_end_frame();
		}
		sf2d_swapbuffers();
	}

	gfx_fini();
}

void menu_test(){
	int total_entries = 3;
	int menupos = 0;
	char* menu_entries[] = {
		"Test 1",
		"Test 2",
		"Test 3"
	};
	char* headerstr = "Pocket-NLSE";

	gfx_init();

	display_menu(menu_entries, total_entries, &menupos, headerstr);

	while(aptMainLoop()){
		hidScanInput();
		if(hidKeysDown() & KEY_A)
			break;

		if(menupos == -1)
			break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		ui_frame("You selected #%d\n", menupos+1);

		sftd_draw_textf(font, 0, 2*fontheight, COLOR_WHITE, fontheight, "Press the A button to continue.\n");
		sf2d_end_frame();
		if(is3dsx){
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_end_frame();
		}
		sf2d_swapbuffers();
	}

	gfx_fini();
}

void mintest(){
	gfx_init();

	while(aptMainLoop()){
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			hidScanInput();

			u32 kDown = hidKeysDown();
			if(kDown & KEY_START)
				break;

			//sf2d_draw_texture(arrow, 0, 0);
			//sftd_draw_textf(font, 0, fontheight, COLOR_WHITE, fontheight, "Some text");
		sf2d_end_frame();
		if(is3dsx){
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_end_frame();
		}
		sf2d_swapbuffers();
	}

	gfx_fini();
}
