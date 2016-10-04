#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

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
#include "actions.h"
#include "menus.h"

#define STACKSIZE (4 * 1024)

static int is_loaded = 0;
static u8* gardenData = NULL;
static u8* gardenPath = NULL;
static FILE* gardenFile = NULL;

static int fontheight = 11;
//static int fontwidth = 7;

int get_loaded_status(){
	if(is_loaded == 0)
		return 0;
	else
		return 1;
}

//load & save
void load_garden(){
	struct stat st;

	while(1){
		char* origpath,* newpath;
		origpath = browse_dir("Load garden.dat");
		stat(origpath, &st);//for size
		newpath = calloc(strlen(origpath)+1, 1);
		strcpy(newpath, origpath);
		if(!strcmp(origpath, "")){
			return;
		}

		char* token = strtok(newpath, "/");
		while(token){
			if(!strcmp(token, "garden.dat") && st.st_size == 522752){
				sf2d_start_frame(GFX_TOP, GFX_LEFT);
					ui_frame();
					sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Loading garden.dat... ");
				sf2d_end_frame();
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_end_frame();
				sf2d_swapbuffers();
				gardenFile = fopen(origpath, "rb");
				gardenPath = (u8*)origpath;
				if(gardenFile != NULL){
					//load entire file into memory
					gardenData = malloc(st.st_size);
					fread(gardenData, st.st_size, 1, gardenFile);
					while(1){
						hidScanInput();

						if(hidKeysDown() & KEY_A)
							break;

						sf2d_start_frame(GFX_TOP, GFX_LEFT);
							ui_frame();
							sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Loading garden.dat... Done!");
							sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
						sf2d_end_frame();
						sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
						sf2d_end_frame();
						sf2d_swapbuffers();
					}
					fclose(gardenFile);
					gardenFile = NULL;
					is_loaded = 1;
				}
				else{
					while(1){
						hidScanInput();

						if(hidKeysDown() & KEY_A)
							break;

						sf2d_start_frame(GFX_TOP, GFX_LEFT);
							ui_frame();
							sftd_draw_textf(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Loading garden.dat... Failed! errno %d", errno);
							sftd_draw_textf(font, 0, fontheight*3, COLOR_WHITE, fontheight, "Path: %s", origpath);
							sftd_draw_text(font, 0, fontheight*5, COLOR_WHITE, fontheight, "Press the A button to continue.");
						sf2d_end_frame();
						sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
						sf2d_end_frame();
						sf2d_swapbuffers();
					}
				}
				fclose(gardenFile);
				gardenFile = NULL;
				
				return;
			}
			token = strtok(NULL, "/");

		}

		while(1){
			hidScanInput();

			if(hidKeysDown() & KEY_A)
				break;
			
			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				ui_frame();
				sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Loading garden.dat... Invalid file!");
				sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
			sf2d_end_frame();
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
	}
}

void save_changes(){
	int i;
	int prompt_res;
	char* path,* filename,* tmpstr;
	
	struct stat path_stat;

	if(gardenData == NULL){
			while(1){
				hidScanInput();

				if(hidKeysDown() & KEY_A)
					break;
				sf2d_start_frame(GFX_TOP, GFX_LEFT);
					ui_frame();
					sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Please load a file first!");
					sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
				sf2d_end_frame();
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_end_frame();
				sf2d_swapbuffers();
			}
		return;
	}

	prompt_res = gfx_prompt3("Overwrite previous file or select a new one?", "A = Overwrite, Y = Select, B = Cancel");
	if(prompt_res == 0){
		path = (char*)gardenPath;
		goto save_changes_end;
	}
	else if(prompt_res == 1){
		return;
	}
	else{
		path = browse_dir("Save garden.dat");
		stat(path, &path_stat); //stat path to check to see if it's a file or directory
		if(S_ISDIR(path_stat.st_mode)){
			kb_init();
			filename = draw_kb("Enter a filename");
			kb_fini();
			if(filename[0] == '\0') //if user pressed B
				return;
			tmpstr = path;
			path = calloc(strlen(path)+strlen(filename)+1, 1);
			sprintf(path, "%s%s", tmpstr, filename);
			goto save_changes_end;
		}
		else{
			gardenFile = fopen(path, "wb");
			goto save_changes_end;
		}
	}

save_changes_end:
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		ui_frame();
		sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Saving (this might take a while)...");
	sf2d_end_frame();
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sf2d_end_frame();
	sf2d_swapbuffers();

	gardenFile = fopen(path, "wb");
	if(gardenFile == NULL){
		while(1){
			hidScanInput();

			if(hidKeysDown() & KEY_A)
				break;

			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				ui_frame();
				sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Error opening file!");
				sftd_draw_text(font, 0, fontheight*3, COLOR_WHITE, fontheight, "Press the A key to continue.");
			sf2d_end_frame();
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
		return;
	}
	//update checksums so the save file isn't labeled as corrupt
	updateChecksum(gardenData, 0x80, 0x1c);
	for(i = 0; i < 4; i++){
		updateChecksum(gardenData, 0xa0+(0x9f10*i), 0x6b64);
		updateChecksum(gardenData, 0xa0+(0x9f10*i)+0x6b68, 0x33a4);
	}
	updateChecksum(gardenData, 0x27ce0, 0x218b0);
	updateChecksum(gardenData, 0x495a0, 0x44b8);
	updateChecksum(gardenData, 0x4da5c, 0x1e420);
	updateChecksum(gardenData, 0x6be80, 0x20);
	updateChecksum(gardenData, 0x6bea4, 0x13af8);
	
	fwrite(gardenData, 522752, 1, gardenFile);
	fclose(gardenFile);
	free(gardenData);
	gardenData = NULL;
	
	while(1){
		hidScanInput();

		if(hidKeysDown() & KEY_A)
			break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame();
			sftd_draw_textf(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Saving (this might take a while)... Done!");
			sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	is_loaded = 0;
}

//menus

void map_menu(){
	int menuindex = 0;
	int menucount = 4;

	char headerstr[] = "Map options";
	char* menu_entries[] = {
		"Grass options",
		"Unbury fossils (not yet working!)",
		"Water flowers (not yet working!)",
		"Map tile editor (not yet working!)"
	};
	
	if(gardenData == NULL){
			while(1){
				hidScanInput();

				if(hidKeysDown() & KEY_A)
					break;
				
				sf2d_start_frame(GFX_TOP, GFX_LEFT);
					ui_frame();
					sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Please load a file first!");
					sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
				sf2d_end_frame();
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_end_frame();
				sf2d_swapbuffers();
			}
		return;
	}

	while(1){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		switch(menuindex){
			case 0:
				grass_menu();
				break;
			case 1:
				unbury_fossils();
				break;
			case 2:
				water_flowers();
				break;
			case 3:
				map_tile_editor();
				break;
		}
	}
}

void player_select(){
	int i;
	int player_offset;
	int menuindex = 0;
	int menucount = 4;

	char headerstr[] = "Select a player";
	char* menu_entries[4];
	
	if(gardenData == NULL){
			while(1){
				hidScanInput();

				if(hidKeysDown() & KEY_A)
					break;
				
				sf2d_start_frame(GFX_TOP, GFX_LEFT);
					ui_frame();
					sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Please load a file first!");
					sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
				sf2d_end_frame();
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_end_frame();
				sf2d_swapbuffers();
			}
		return;
	}

	for(i = 0; i < 4; i++){
		menu_entries[i] = calloc(11, 1);
		player_offset = (0x20+0x9f10*i);
		menu_entries[i] = (char*)get_ustring(gardenData, player_offset+0x55a8, 10);
		if(get_ustring(gardenData, (0x20+0x9f10*(i+1))+0x55a8, 1)[0] == '\0')
			break;
	}
	menucount = i+1;


	while(1){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		player_menu(menuindex);
	}

	for(i = 0; i < menucount; i++){
		free(menu_entries[i]);
	}
}

void villager_select(){
	int i;
	int villager_offset;
	int menuindex = 0;
	int menucount = 4;

	char headerstr[] = "Select a villager";
	char* menu_entries[10]; //max of 10 villagers per town
	
	if(gardenData == NULL){
			while(1){
				hidScanInput();

				if(hidKeysDown() & KEY_A)
					break;
				
				sf2d_start_frame(GFX_TOP, GFX_LEFT);
					ui_frame();
					sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Please load a file first!");
					sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
				sf2d_end_frame();
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_end_frame();
				sf2d_swapbuffers();
			}
		return;
	}

	for(i = 0; i < 4; i++){
		menu_entries[i] = calloc(11, 1);
		villager_offset = (0x20+0x9f10*i);
		menu_entries[i] = (char*)get_ustring(gardenData, villager_offset+0x55a8, 10);
	}

	while(1){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		//villager_menu(menuindex);
	}

	for(i = 0; i < 4; i++){
		free(menu_entries[i]);
	}
}

void misc_menu(){
	int menuindex = 0;
	int menucount = 1;

	char headerstr[] = "Misc options";
	char* menu_entries[] = {
		"Unlock all PWPs"
	};
	
	if(gardenData == NULL){
			while(1){
				hidScanInput();

				if(hidKeysDown() & KEY_A)
					break;
				
				sf2d_start_frame(GFX_TOP, GFX_LEFT);
					ui_frame();
					sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Please load a file first!");
					sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
				sf2d_end_frame();
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_end_frame();
				sf2d_swapbuffers();
			}
		return;
	}

	while(1){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		switch(menuindex){
			case 0:
				unlock_all_pwps();
				break;
		}
	}
}


//map menu
void grass_menu(){
	int menuindex = 0;
	int menucount = 2;

	char headerstr[] = "Select an option";
	char* menu_entries[2] = {
		"Revive grass",
		"Strip grass"
	};
	
	if(gardenData == NULL){
			while(1){
				hidScanInput();

				if(hidKeysDown() & KEY_A)
					break;
				
				sf2d_start_frame(GFX_TOP, GFX_LEFT);
					ui_frame();
					sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Please load a file first!");
					sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
				sf2d_end_frame();
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_end_frame();
				sf2d_swapbuffers();
			}
		return;
	}

	while(1){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame();
			if(menuindex == 0)
				sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Reviving grass...");
			else
				sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Stripping grass...");
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_end_frame();
		sf2d_swapbuffers();
		if(menuindex == 0)
			set_grass(gardenData, 0xFF);
		else
			set_grass(gardenData, 0x00);
		
		while(1){
			hidScanInput();

			if(hidKeysDown() & KEY_A)
				break;
			
			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				ui_frame();
				if(menuindex == 0)
					sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Reviving grass... Done!");
				else
					sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Stripping grass... Done!");
				sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
			sf2d_end_frame();
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
	}
}

void unbury_fossils(){
	//TODO
}

void water_flowers(){
	//TODO
}

void map_tile_editor(){
	//TODO
}


//player select & player menu
void player_menu(int player){
	int menuindex = 0;
	int menucount = 7;

	char headerstr[] = "Select an option";
	char* menu_entries[] = {
		"Max bank",
		"Change gender (here be dragons!)",
		"Change tan shade",
		"Change hair style",
		"Change hair color",
		"Change eye color",
		"Change face type"
	};

	while(1){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		switch(menuindex){
			case 0:
				max_bank(player);
				break;
			case 1:
				change_gender(player);
				break;
			case 2:
				change_tan(player);
				break;
			case 3:
				change_hair_style(player);
				break;
			case 4:
				change_hair_color(player);
				break;
			case 5:
				change_eye_color(player);
				break;
			case 6:
				change_face(player);
				break;
		}
	}
}

void max_bank(int player){
	int i;
	//original IDs
	int bank_ids[] = {0x78,0x56,0xf9,0x8c,0x36,0x86,0x11,0x0d};
	int bank_offset = (0x20+0x9f10*player)+0x6b6c;

	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		ui_frame();
		sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Maxing bank... ");
	sf2d_end_frame();
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sf2d_end_frame();
	sf2d_swapbuffers();

	for(i = 0; i < 8; i++){
		storeByte(gardenData, bank_offset+i, bank_ids[i]);
	}
	
	while(1){
		hidScanInput();

		if(hidKeysDown() & KEY_A)
			break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame();
			sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Maxing bank... Done!");
			sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
}

void change_gender(int player){
	int player_offset = 0x20+0x9f10*player;
	int menuindex = 0;
	int menucount = 2;
	char* str = NULL;

	char headerstr[] = "Select a gender";
	char* menu_entries[] = {
		"Male",
		"Female"
	};

	while(1){
		hidScanInput();

		if(hidKeysDown() & KEY_A)
			break;
		else if(hidKeysDown() & KEY_B)
			return;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame();
			sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Warning! This will make your villagers forget who you");
			sftd_draw_text(font, 0, fontheight*3, COLOR_WHITE, fontheight, "are! Press A if you still wish to continue, otherwise,");
			sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "press B.");
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}

	while(1){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		switch(menuindex){
			case 0:
				storeByte(gardenData, player_offset+0x55ba, 0x00);
				break;
			case 1:
				storeByte(gardenData, player_offset+0x55ba, 0x01);
				break;
		}

		while(1){
			hidScanInput();

			if(hidKeysDown() & KEY_A)
				break;

			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				ui_frame();
				if(menuindex == 0)
					str = "Changing gender to male... ";
				else
					str = "Changing gender to female... ";
				sftd_draw_textf(font, 0, fontheight*2, COLOR_WHITE, fontheight, "%s Done!", str);
				sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
			sf2d_end_frame();
			sf2d_start_frame(GFX_BOTTOM, GFX_TOP);
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
	}
}

void change_tan(int player){
	int player_offset = 0x20+0x9f10*player;
	int menuindex = 0;
	int menucount = 16;

	char headerstr[] = "Select tan value";
	char* menu_entries[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};

	while(1){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		storeByte(gardenData, player_offset+0x8, (u8)menuindex);

		while(1){
			hidScanInput();

			if(hidKeysDown() & KEY_A)
				break;

			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				ui_frame();
				sftd_draw_textf(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Changing tan value to %d... Done!", menuindex);
				sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
			sf2d_end_frame();
			sf2d_start_frame(GFX_BOTTOM, GFX_TOP);
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
	}
}

void change_hair_style(int player){
	int i;
	int player_offset = 0x20+0x9f10*player;
	int menuindex = 0;
	int menucount = 34;

	char headerstr[] = "Select a hair style";
	char* menu_entries[34]; //0-16 Male, 17-34 Female

	for(i = 0; i < 17; i++){
		menu_entries[i] = calloc(8, 1); //Male + ' ' + 2 chars + \0 = 8
		snprintf(menu_entries[i], 8, "Male %d", i);
		menu_entries[i+17] = calloc(10, 1);
		snprintf(menu_entries[i+17], 10, "Female %d", i);
	}
	menu_entries[16] = calloc(17, 1);
	snprintf(menu_entries[16], 16, "Male (beadhead)");
	menu_entries[16+17] = calloc(19, 1);
	snprintf(menu_entries[16+17], 18, "Female (beadhead)");

	while(1){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		storeByte(gardenData, player_offset+0x04, (u8)menuindex);

		while(1){
			hidScanInput();

			if(hidKeysDown() & KEY_A)
				break;

			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				ui_frame();
				sftd_draw_textf(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Changing hair style to %s... Done!", menu_entries[menuindex]);
				sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
			sf2d_end_frame();
			sf2d_start_frame(GFX_BOTTOM, GFX_TOP);
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
	}
	//TODO free dat shit!
}

void change_hair_color(int player){
	int player_offset = 0x20+0x9f10*player;
	int menuindex = 0;
	int menucount = 16;

	char headerstr[] = "Select a hair color";
	char* menu_entries[] = {
		"Dark brown",
		"Light brown",
		"Orange",
		"Light blue",
		"Gold",
		"Light green",
		"Pink",
		"White",
		"Black",
		"Auburn",
		"Red",
		"Dark blue",
		"Blonde",
		"Dark green",
		"Light purple",
		"Ash brown"
	};

	while(1){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		storeByte(gardenData, player_offset+0x05, (u8)menuindex);

		while(1){
			hidScanInput();

			if(hidKeysDown() & KEY_A)
				break;

			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				ui_frame();
				sftd_draw_textf(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Changing hair color to %s... Done!", menu_entries[menuindex]);
				sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
			sf2d_end_frame();
			sf2d_start_frame(GFX_BOTTOM, GFX_TOP);
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
	}
}

void change_eye_color(int player){
	int player_offset = 0x20+0x9f10*player;
	int menuindex = 0;
	int menucount = 8;

	char headerstr[] = "Select a eye color";
	char* menu_entries[] = {"0", "1", "2", "3", "4", "5", "6", "7"};

	while(1){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		storeByte(gardenData, player_offset+0x07, (u8)menuindex);

		while(1){
			hidScanInput();

			if(hidKeysDown() & KEY_A)
				break;

			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				ui_frame();
				sftd_draw_textf(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Changing eye color to %d... Done!", menuindex);
				sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
			sf2d_end_frame();
			sf2d_start_frame(GFX_BOTTOM, GFX_TOP);
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
	}
}

void change_face(int player){
	int player_offset = 0x20+0x9f10*player;
	int menuindex = 0;
	int menucount = 12;

	char headerstr[] = "Select a face type";
	char* menu_entries[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"};

	while(1){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		storeByte(gardenData, player_offset+0x06, (u8)menuindex);

		while(1){
			hidScanInput();

			if(hidKeysDown() & KEY_A)
				break;

			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				ui_frame();
				sftd_draw_textf(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Changing face type to %d... Done!", menuindex);
				sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
			sf2d_end_frame();
			sf2d_start_frame(GFX_BOTTOM, GFX_TOP);
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
	}
}


//villager select & villager menu
void villager_menu(){
	//TODO
}

void box_menu(){
	//TODO
}

void reset_villager(){
	//TODO
}

void change_villager(){
	//TODO
}


//misc menu
void unlock_all_pwps(){
	unsigned int i;
	char* str;

	if(gardenData == NULL){
		while(1){
			hidScanInput();

			if(hidKeysDown() & KEY_A)
				break;

			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				ui_frame();
				sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Please load a file first!");
				sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A key to continue.");
			sf2d_end_frame();
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
		return;
	}
	u8 pwp_ids[]={
		0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		0xff,0xff,0xff,0xff,0x2a,0xd6,0xe4,0x58
	};

	for(i = 0; i < sizeof(pwp_ids)/sizeof(u8); i++){
		storeByte(gardenData, 0x04d9c8+i, pwp_ids[i]);
	}
	
	while(1){
		hidScanInput();

		if(hidKeysDown() & KEY_A)
			break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame();
			str = "Unlocking all PWPs... ";
			sftd_draw_textf(font, 0, fontheight*2, COLOR_WHITE, fontheight, "%s Done!", str);
			sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
}


//list test
void list_test(){
	unsigned int i;
	int menuindex = 0;
	int menucount;

	char headerstr[] = "Number list";
	char* list[27];

	for(i = 0; i < sizeof(list)/sizeof(list[0]); i++){
		list[i] = calloc(3, 1);
		sprintf(list[i], "%d", i);
	}

	while(1){
		menucount = sizeof(list)/sizeof(list[0]);

		display_menu(list, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		while(1){
			hidScanInput();

			if(hidKeysDown() & KEY_A)
				break;

			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				ui_frame();
				sftd_draw_textf(font, 0, fontheight*2, COLOR_WHITE, fontheight, "menuindex: %d", menuindex);
				sftd_draw_text(font, 0, fontheight*4, COLOR_WHITE, fontheight, "Press the A button to continue.");
			sf2d_end_frame();
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
	}

	for(i = 0; i < sizeof(list)/sizeof(list[0]); i++){
		free(list[i]);
	}
	while(1){
		hidScanInput();

		if(hidKeysDown() & KEY_A)
			break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame();
			sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Press the A button to continue.");
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
}
