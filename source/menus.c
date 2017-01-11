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

#include "constants.h"
#include "common.h"
#include "ids.h"
#include "gfx.h"
#include "menu.h"
#include "dir.h"
#include "ui.h"
#include "kb.h"
#include "actions.h"
#include "acres.h"
#include "fs.h"
#include "backup.h"
#include "menus.h"

static int is_loaded = 0;
static u8* gardenPath = NULL;
static Handle sdmc_file_handle = 0;

static int fontheight = 11;

int get_loaded_status(){
	if(is_loaded == 0)
		return 0;
	else
		return 1;
}

//load & save
void load_menu(){
	int menuindex = 0;
	int menucount = 2;

	char headerstr[] = "Load/restore options";
	char* menu_entries[] = {
		"Dump save & create backup",
		"Restore from backup"
	};

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		switch(menuindex){
			case 0:
				dump_and_backup_garden();
				break;
			case 1:
				restore_backup();
				break;
		}
	}
}

void save_menu(){
	int menuindex = 0;
	int menucount = 2;

	char headerstr[] = "Save/inject options";
	char* menu_entries[] = {
		"Inject changes into game",
		"Save garden_plus.dat to file",
	};

	if(!gardenData){
		gfx_waitmessage("Please dump or load a file first!");
		return;
	}

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		switch(menuindex){
			case 0:
				inject_changes();
				break;
			case 1:
				save_garden_file();
				break;
		}
	}
}

//load menu
void load_garden_file(){
	Result ret;
	u64 size;
	u32 read;
	char* path;

	while(aptMainLoop()){
		path = browse_dir("Load garden_plus.dat");
		if(!strcmp(path, "")){
			return;
		}
		ret = FSUSER_OpenFile(&sdmc_file_handle, sdmc_arch, fsMakePath(PATH_ASCII, path), FS_OPEN_WRITE, 0);
		if(ret){
			gfx_error(ret, __FILENAME__, __LINE__);
			return;
		}
		FSFILE_GetSize(sdmc_file_handle, &size);

		if(size == SIZE_GARDEN){
			gfx_displaymessage("Loading garden_plus.dat...");
			gardenPath = (u8*)path;
			//load entire file into memory
			gardenData = malloc(size);
			ret = FSFILE_Read(sdmc_file_handle, &read, 0, gardenData, SIZE_GARDEN);
			if(ret){
				gfx_error(ret, __FILENAME__, __LINE__);
				FSFILE_Close(sdmc_file_handle);
			}
			gfx_waitmessage("Loading garden_plus.dat... Done!");
			is_loaded = 1;
			
			return;
		}
		else{
			gfx_waitmessage("Loading garden_plus.dat... Invalid file!");
			continue;
		}
	}
}

void dump_and_backup_garden(){
	u64 size;

	gfx_displaymessage("Dumping save into memory...");
		gardenData = (u8*)file_to_buffer(game_arch, "/", "garden_plus.dat");
	gfx_displaymessage("Backing up to \"backup\"...");
		size = filesize_to_u64(game_arch, "/garden_plus.dat");
		FSUSER_CreateDirectory(sdmc_arch, fsMakePath(PATH_ASCII, "/Pocket-NLSE/Saves/backup"), 0);
		buffer_to_file(sdmc_arch, (char*)gardenData, size, "/Pocket-NLSE/Saves/backup/", "garden_plus.dat");
	gfx_waitmessage("Dumped save data to memory. Dumped memory to \"backup\"");

	is_loaded = 1;
}

void restore_backup(){
	u64 size;

	gfx_displaymessage("Restoring backup from \"backup\"...");
		gardenData = (u8*)file_to_buffer(sdmc_arch, "/Pocket-NLSE/Saves/backup/", "garden_plus.dat");
		size = filesize_to_u64(sdmc_arch, "/Pocket-NLSE/Saves/backup/garden_plus.dat");
		buffer_to_file(game_arch, (char*)gardenData, size, "/", "garden_plus.dat");
	gfx_waitmessage("Restoring backup from \"backup\"... Done!");
}

//save menu
void inject_changes(){
	u64 size;
	
	if(!gardenData){
		gfx_waitmessage("Please dump your save first!");
		return;
	}

	gfx_displaymessage("Injecting changes into game...");
	writeChecksums(gardenData);
	size = filesize_to_u64(sdmc_arch, "/Pocket-NLSE/Saves/backup/garden_plus.dat");
	buffer_to_file(game_arch, (char*)gardenData, size, "/", "garden_plus.dat");
	gfx_waitmessage("Injecting changes into game... Done!");

	FSFILE_Close(sdmc_file_handle);
	free(gardenData);
	gardenData = NULL;

	is_loaded = 0;
}

void save_garden_file(){
	Result ret;
	u32 written;
	
	if(sdmc_file_handle == 0){
		if(gardenData)
			gfx_waitmessage("Please inject the save instead!");
		else
			gfx_waitmessage("Please load a file first!");
		return;
	}

	if(gfx_prompt("Overwrite previous file?", NULL))
		return;

	gfx_displaymessage("Saving...");

	//update checksums so the save file isn't labeled as corrupt
	writeChecksums(gardenData);
	
	ret = FSFILE_Write(sdmc_file_handle, &written, 0, gardenData, SIZE_GARDEN, FS_WRITE_FLUSH);
	if(ret)
		gfx_error(ret, __FILENAME__, __LINE__);
	ret = FSFILE_Close(sdmc_file_handle);
	if(ret)
		gfx_error(ret, __FILENAME__, __LINE__);
	free(gardenData);
	gardenData = NULL;

	gfx_waitmessage("Saving... Done!");
	is_loaded = 0;
}

//menus

void map_menu(){
	int menuindex = 0;
	int menucount = 2;

	char headerstr[] = "Map options";
	char* menu_entries[] = {
		"Grass options",
		"Unbury fossils",
		"Water flowers (not yet working!)",
		"Map tile editor (not yet working!)"
	};
	
	if(gardenData == NULL){
		gfx_waitmessage("Please load a file first!");
		return;
	}

	while(aptMainLoop()){
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
		gfx_waitmessage("Please load a file first!");
		return;
	}

	for(i = 0; i < 4; i++){
		menu_entries[i] = calloc(11, 1);
		player_offset = (OFFSET_PLAYERS+SIZE_PLAYER*i);
		menu_entries[i] = (char*)get_ustring(gardenData, player_offset+OFFSET_PLAYER_NAME, 10);
		if(get_ustring(gardenData, (OFFSET_PLAYERS+SIZE_PLAYER*(i+1))+OFFSET_PLAYER_NAME, 1)[0] == '\0')
			break;
	}
	menucount = i+1;


	while(aptMainLoop()){
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
	u16 id[10]; //villager ID
	u32 status;
	bool is_boxed[10];
	int menuindex = 0;
	int menucount = 0;

	char headerstr[] = "Select a villager";
	char* menu_entries[10]; //max of 10 villagers per town
	
	if(gardenData == NULL){
		gfx_waitmessage("Please load a file first!");
		return;
	}

	while(aptMainLoop()){
		for(i = 0; i < 10; i++){
			villager_offset = OFFSET_VILLAGERS+SIZE_VILLAGER*i;
			id[i] = readByte2(gardenData, villager_offset);
			status = readByte4(gardenData, villager_offset+OFFSET_VILLAGER_STATUS);
			if(id[i] == 0xffff || id[i] > 398){
				if(id[i] == 0xffff){
					menu_entries[i] = "(Empty slot!)";
					menucount = i+1;
				}
				else{
					menucount = i+1;
					break;
				}
			}
			if(status == (status | 0x00000001))
				is_boxed[i] = true;
			else
				is_boxed[i] = false;

			if(id[i] < 398 && i == 9)
				menucount = i+1;

			if(is_boxed[i]){
				menu_entries[i] = calloc(strlen(villagers[id[i]].name)+8+1, 1); //" (boxed)" = 8
				strcat(menu_entries[i], villagers[id[i]].name);
				strcat(menu_entries[i], " (boxed)");
			}
			else if(id[i] != 0xffff){
				menu_entries[i] = villagers[id[i]].name;
			}
		}

		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		if(id[menuindex] != 0xffff)
			villager_menu(menuindex);
		else
			gfx_waitmessage("Error! Can't select empty slot right now!");
	}

	for(i = 0; i < 10; i++){
		if(is_boxed[i]){
			free(menu_entries[i]);
		}
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
		gfx_waitmessage("Please load a file first!");
		return;
	}

	while(aptMainLoop()){
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
		gfx_waitmessage("Please load a file first!");
		return;
	}

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		if(menuindex == 0)
			gfx_displaymessage("Reviving grass...");
		else
			gfx_displaymessage("Stripping grass...");
		if(menuindex == 0)
			set_grass(gardenData, 0xFF);
		else
			set_grass(gardenData, 0x00);
		
		if(menuindex == 0)
			gfx_waitmessage("Reviving grass... Done!");
		else
			gfx_waitmessage("Stripping grass... Done!");
	}
}

void unbury_fossils(){
	int i;
	int numfossils = 0;

	gfx_displaymessage("Finding buried fossils...");
	for(i = 0; i < SIZE_MAP_ITEMS; i++){
		if(map_items[i].itemid == 0x202a && map_items[i].flag2 == 0x80){
			numfossils++;
			storeByte(gardenData, OFFSET_MAP_ITEMS+i*4+3, 0x00);
		}
	}
	gfx_waitmessage("%d fossils unburied!", numfossils);
}

void water_flowers(){
	//TODO
}

void map_tile_editor(){
	//TODO
	int i, j;
	u8 acres[7*6];
	u8 current_acre;
	u8 acre_cur_x = 0;
	u8 acre_cur_y = 0;

	u8 tile_cur_x = 0;
	u8 tile_cur_y = 0;

	//get acre bytes into acres
	j = 0;
	for(i = 0; i < 7*6*2; i++){
		if(i == 7*6*2-1){
			acres[j] = readByte1(gardenData, OFFSET_MAP_ACRES+i-1);
			j++;
		}
		else if(i % 2 == 0){
			acres[j] = readByte1(gardenData, OFFSET_MAP_ACRES+i-2);
			j++;
		}
	}

	while(aptMainLoop()){
		hidScanInput();

		if(hidKeysDown() & KEY_B)
			break;
		else if(hidKeysHeld() & KEY_UP && acre_cur_y > 0){
			acre_cur_y--;
			svcSleepThread(0.25*SECOND_IN_NS);
		}
		else if(hidKeysHeld() & KEY_RIGHT && acre_cur_x < 6){
			acre_cur_x++;
			svcSleepThread(0.25*SECOND_IN_NS);
		}
		else if(hidKeysHeld() & KEY_DOWN && acre_cur_y < 5){
			acre_cur_y++;
			svcSleepThread(0.25*SECOND_IN_NS);
		}
		else if(hidKeysHeld() & KEY_LEFT && acre_cur_x > 0){
			acre_cur_x--;
			svcSleepThread(0.25*SECOND_IN_NS);
		}

		//top screen (acres)
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame("Use dPad to select acre. Touch tiles to change item");
			sf2d_draw_rectangle(88-3, 36-3, 223+6, 179+6, COLOR_BLACK);
			sf2d_draw_rectangle(88, 36, 223, 179, RGBA8(0x77, 0x77, 0x77, 0xFF));
			
			//draw acres
			for(i = 0; i < 6; i++){
				for(j = 0; j < 7; j++){
					if(i == 0 && (acre_textures[acres[(i*7)+(j+1)]] != NULL)) //first row
						sf2d_draw_texture_scale(acre_textures[acres[(i*7)+(j+1)]], 88+j*32, 36, 0.49, 0.49);
					else if(acre_textures[acres[(i*7)+(j+1)]] != NULL) //all other rows
						sf2d_draw_texture_scale(acre_textures[acres[(i*7)+(j+1)]], 88+j*32, 24+i*32, 0.49, 0.49);
				}
			}
			//draw acre cursor overlay
			if(acre_cur_y == 0)
				sf2d_draw_rectangle(88+32*acre_cur_x, 36, 31, 19, RGBA8(0x00, 0x00, 0xFF, 0x088));
			else
				sf2d_draw_rectangle(88+32*acre_cur_x, 24+32*acre_cur_y, 31, 31, RGBA8(0x00, 0x00, 0xFF, 0x88));
			current_acre = (acre_cur_y*7)+(acre_cur_x+1);
		sf2d_end_frame();
		
		//bottom screen (tiles)
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_rectangle(56-3, 15-3, 208+6, 208+6, COLOR_BLACK);
			sf2d_draw_rectangle(56, 15, 208, 208, RGBA8(0x77, 0x77, 0x77, 0xFF));
			for(i = 0; i < 16; i++){ //y
				for(j = 0; j < 16; j++){ //x
					sf2d_draw_texture_part_scale(acre_textures[acres[current_acre]], 56+j*13, 15+i*13, j*4, i*4, 4, 4, 3.00, 3.00);
				}
			}
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
}


//player select & player menu
void player_menu(int player){
	int menuindex = 0;
	int menucount = 6;

	char headerstr[] = "Select an option";
	char* menu_entries[] = {
		"Bank options",
		"Change tan shade",
		"Change hair style",
		"Change hair color",
		"Change eye color",
		"Change face type",
		"Change gender (here be dragons!)",
		"Item editing options (not fully functional)",
	};

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		switch(menuindex){
			case 0:
				bank_menu(player);
				break;
			case 1:
				change_tan(player);
				break;
			case 2:
				change_hair_style(player);
				break;
			case 3:
				change_hair_color(player);
				break;
			case 4:
				change_eye_color(player);
				break;
			case 5:
				change_face(player);
				break;
			case 6:
				change_gender(player);
				break;
			case 7:
				item_menu(player);
				break;
		}
	}
}

void bank_menu(int player){
	int menuindex = 0;
	int menucount;
	if(devmode)
		menucount = 4;
	else
		menucount = 3;

	char headerstr[] = "Select an option";
	char* menu_entries[] = {
		"Max bank",
		"Wipe bank",
		"Set bank balance",
		"View bank IDs"
	};

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		switch(menuindex){
			case 0:
				max_bank(player);
				break;
			case 1:
				wipe_bank(player);
				break;
			case 2:
				select_bank_balance(player);
				break;
			case 3:
				view_bank_ids(player);
				break;
		}
	}
}

void select_bank_balance(int player){
	int menuindex = 0;
	int menucount = 10;

	char headerstr[] = "Select an option";
	char* menu_entries[] = {
		"10,000 bells",
		"25,000 bells",
		"50,000 bells",
		"100,000 bells",
		"250,000 bells",
		"500,000 bells",
		"1,000,000 bells",
		"5,000,000 bells",
		"10,000,000 bells",
		"25,000,000 bells"
	};

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
	
		set_bank_balance(player, menuindex);
	}
}

void set_bank_balance(int player, int menuindex){
	int i;
	char* str = NULL;
	
	int bank_offset = OFFSET_PLAYER_BANK+OFFSET_PLAYERS+SIZE_PLAYER*player;
	int bank_ids_10k[] = {0xac, 0x18, 0x8f, 0x9d, 0x5b, 0x11, 0x14, 0xaa};
	int bank_ids_25k[] = {0xe6, 0xcb, 0x78, 0xf4, 0x04, 0xa7, 0x17, 0xd7};
	int bank_ids_50k[] = {0x8d, 0x47, 0xe7, 0x12, 0x4c, 0xee, 0x0b, 0x87};
	int bank_ids_100k[] = {0xc2, 0xa6, 0xf1, 0x18, 0x4f, 0x71, 0x18, 0x2b};
	int bank_ids_250k[] = {0x98, 0xd2, 0xce, 0xf1, 0x67, 0xa8, 0x00, 0xe3};
	int bank_ids_500k[] = {0x11, 0xb3, 0x06, 0xe4, 0x46, 0x20, 0x01, 0x68};
	int bank_ids_1m[] = {0xf2, 0x78, 0x52, 0x7e, 0xb5, 0x2e, 0x08, 0xf4};
	int bank_ids_5m[] = {0x3d, 0xa2, 0x18, 0x94, 0xb6, 0x46, 0x06, 0x45};
	int bank_ids_10m[] = {0x1f, 0x4f, 0xfb, 0x62, 0xf5, 0x72, 0x05, 0x85};
	int bank_ids_25m[] = {0x69, 0xa7, 0x62, 0x09, 0x04, 0x3e, 0x00, 0x35};
	//int bank_ids_50m[] = {};
	//int bank_ids_100m[] = {};
	//int bank_ids_250m[] = {};
	//int bank_ids_500m[] = {};

	switch(menuindex){
		case 0:
			str = "10,000";
			for(i = 0; i < 8; i++)
				storeByte(gardenData, bank_offset+i, bank_ids_10k[i]);
			break;
		case 1:
			str = "25,000";
			for(i = 0; i < 8; i++)
				storeByte(gardenData, bank_offset+i, bank_ids_25k[i]);
			break;
		case 2:
			str = "50,000";
			for(i = 0; i < 8; i++)
				storeByte(gardenData, bank_offset+i, bank_ids_50k[i]);
			break;
		case 3:
			str = "100,000";
			for(i = 0; i < 8; i++)
				storeByte(gardenData, bank_offset+i, bank_ids_100k[i]);
			break;
		case 4:
			str = "250,000";
			for(i = 0; i < 8; i++)
				storeByte(gardenData, bank_offset+i, bank_ids_250k[i]);
			break;
		case 5:
			str = "500,000";
			for(i = 0; i < 8; i++)
				storeByte(gardenData, bank_offset+i, bank_ids_500k[i]);
			break;
		case 6:
			str = "1,000,000";
			for(i = 0; i < 8; i++)
				storeByte(gardenData, bank_offset+i, bank_ids_1m[i]);
			break;
		case 7:
			str = "5,000,000";
			for(i = 0; i < 8; i++)
				storeByte(gardenData, bank_offset+i, bank_ids_5m[i]);
			break;
		case 8:
			str = "10,000,000";
			for(i = 0; i < 8; i++)
				storeByte(gardenData, bank_offset+i, bank_ids_10m[i]);
			break;
		case 9:
			str = "25,000,000";
			for(i = 0; i < 8; i++)
				storeByte(gardenData, bank_offset+i, bank_ids_25m[i]);
			break;
	}

	gfx_waitmessage("Setting bank balance to %s... Done!", str);
}

void max_bank(int player){
	int i;
	//original IDs
	int bank_ids[] = {0x78,0x56,0xf9,0x8c,0x36,0x86,0x11,0x0d};
	int bank_offset = OFFSET_PLAYER_BANK+OFFSET_PLAYERS+SIZE_PLAYER*player;

	gfx_displaymessage("Maxing bank...");

	for(i = 0; i < 8; i++){
		storeByte(gardenData, bank_offset+i, bank_ids[i]);
	}

	gfx_waitmessage("Maxing bank... Done!");
}

void wipe_bank(int player){
	int i;
	//original IDs
	int bank_ids[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	int bank_offset = OFFSET_PLAYER_BANK+OFFSET_PLAYERS+SIZE_PLAYER*player;

	gfx_displaymessage("Wiping bank...");

	for(i = 0; i < 8; i++){
		storeByte(gardenData, bank_offset+i, bank_ids[i]);
	}
	
	gfx_waitmessage("Wiping bank... Done!");
}

void view_bank_ids(int player){
	int i;
	//original IDs
	int bank_ids[8];
	int bank_offset = OFFSET_PLAYER_BANK+OFFSET_PLAYERS+SIZE_PLAYER*player;

	for(i = 0; i < 8; i++){
		bank_ids[i] = readByte1(gardenData, bank_offset+i);
	}
	
	while(aptMainLoop()){
		hidScanInput();

		if(hidKeysDown() & KEY_A)
			break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame("View bank IDs");
			for(i = 0; i < 8; i++){
				sftd_draw_textf(font, 0, fontheight*(2+i), COLOR_WHITE, fontheight, "%d: 0x%02x", i, bank_ids[i]);
			}
			sftd_draw_text(font, 0, fontheight*11, COLOR_WHITE, fontheight, "Press the A button to continue.");
		sf2d_end_frame();
		if(is3dsx){
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_end_frame();
		}
		sf2d_swapbuffers();
	}
}

void item_menu(int player){
	int menuindex = 0;
	int menucount = 4;

	char headerstr[] = "Item editing options";

	char* menu_entries[] = {
		"Edit pockets",
		"Edit dressers",
		"Edit island box",
		"Edit secret storage room"
	};

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		switch(menuindex){
			case 0:
				edit_pockets(player);
				break;
			case 1:
				edit_dressers(player);
				break;
			case 2:
				edit_island_box(player);
				break;
			case 3:
				edit_secret_storage_room(player);
				break;
		}
	}
}

void edit_pockets(int player){
	int menuindex = 0;
	int menucount = SIZE_PLAYER_POCKETS;
	int i;

	char headerstr[] = "Select a pocket slot to edit its contents";

	char* menu_entries[SIZE_PLAYER_POCKETS];

	for(i = 0; i < SIZE_PLAYER_POCKETS; i++){
		if(pockets[player][i].itemid != 0x7ffe){
			menu_entries[i] = calloc(7, 1); //0xffff + '\0' = 7
			sprintf(menu_entries[i], "0x%x", pockets[player][i].itemid);
		}
		else{
			menu_entries[i] = "(None)";
		}
	}

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		gfx_waitmessage("Item ID: 0x%x", pockets[player][menuindex].itemid);
	}

	for(i = 0; i < SIZE_PLAYER_POCKETS; i++){
		if(pockets[player][i].itemid != 0x7ffe)
			free(menu_entries[i]);
	}
}

void edit_dressers(int player){
	int menuindex = 0;
	int menucount = SIZE_PLAYER_DRESSERS;
	int i;

	char headerstr[] = "Select a dresser slot to edit its contents";

	char* menu_entries[SIZE_PLAYER_DRESSERS];

	for(i = 0; i < SIZE_PLAYER_DRESSERS; i++){
		if(dressers[player][i].itemid != 0x7ffe){
			menu_entries[i] = calloc(7, 1); //0xffff + '\0' = 7
			sprintf(menu_entries[i], "0x%x", dressers[player][i].itemid);
		}
		else{
			menu_entries[i] = "(None)";
		}
	}

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		gfx_waitmessage("Item ID: 0x%x", dressers[player][menuindex].itemid);
	}
	for(i = 0; i < SIZE_PLAYER_DRESSERS; i++){
		if(dressers[player][i].itemid != 0x7ffe)
			free(menu_entries[i]);
	}
}

void edit_island_box(int player){
	int menuindex = 0;
	int menucount = SIZE_PLAYER_ISLANDBOX;
	int i;

	char headerstr[] = "Select a island box slot to edit its contents";

	char* menu_entries[SIZE_PLAYER_ISLANDBOX];

	for(i = 0; i < SIZE_PLAYER_ISLANDBOX; i++){
		if(islandbox[player][i].itemid != 0x7ffe){
			menu_entries[i] = calloc(7, 1); //0xffff + '\0' = 7
			sprintf(menu_entries[i], "0x%x", islandbox[player][i].itemid);
		}
		else{
			menu_entries[i] = "(None)";
		}
	}

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		gfx_waitmessage("Item ID: 0x%x", islandbox[player][menuindex].itemid);
	}
	for(i = 0; i < SIZE_PLAYER_ISLANDBOX; i++){
		if(islandbox[player][i].itemid != 0x7ffe)
			free(menu_entries[i]);
	}
}

void edit_secret_storage_room(int player){
	int menuindex = 0;
	int menucount = SIZE_PLAYER_STORAGE;
	int i;

	char headerstr[] = "Select a secret storage slot to edit its contents";

	char* menu_entries[SIZE_PLAYER_STORAGE];

	for(i = 0; i < SIZE_PLAYER_STORAGE; i++){
		if(storage[player][i].itemid != 0x7ffe){
			menu_entries[i] = calloc(7, 1); //0xffff + '\0' = 7
			sprintf(menu_entries[i], "0x%x", storage[player][i].itemid);
		}
		else{
			menu_entries[i] = "(None)";
		}
	}

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		gfx_waitmessage("Item ID: 0x%x", storage[player][menuindex].itemid);
	}
	for(i = 0; i < SIZE_PLAYER_STORAGE; i++){
		if(storage[player][i].itemid != 0x7ffe)
			free(menu_entries[i]);
	}
}

void change_gender(int player){
	int player_offset = OFFSET_PLAYERS+SIZE_PLAYER*player;
	int menuindex = 0;
	int menucount = 2;
	char* str = NULL;
	u8 gender = 0x00;
	u16 player_id[11];
	u32 offset;
	bool found;
	u32* player_id_references = NULL;
	int i, j = 0;

	char headerstr[] = "Select a gender";
	char* menu_entries[] = {
		"Male",
		"Female"
	};

	gfx_prompt("Warning! This will make your villagers forget who you are!", "Press A to continue, otherwise, press B.");

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		switch(menuindex){
			case 0:
				gender = 0x00;
				break;
			case 1:
				gender = 0x01;
				break;
		}
		for(i = 0; i < 11; i++){
			player_id[i] = readByte2(gardenData, player_offset+OFFSET_PLAYER_ID1+i*2);
		}
		if(player_id[0] != 0x0000){
			for(offset = 0; offset < 522624-11*2; offset+=2){
				found = true;
				for(i = 0; i < 11 && found; i++){
					if(readByte2(gardenData,offset+i*2) != player_id[i])
						found = false;
				}
			}
			if(found){
				j++;
				player_id_references = realloc(player_id_references, j*sizeof(u32));
				player_id_references[j-1] = offset;
				offset += 11*2;
			}
		}
		for(i = 0; i < j; i++){
			storeByte(gardenData, player_id_references[i]+20, gender);
		}
		storeByte(gardenData, player_offset+OFFSET_PLAYER_GENDER, gender);

		if(menuindex == 0)
			str = "Changing gender to male... ";
		else
			str = "Changing gender to female... ";
		gfx_waitmessage("%s Done!", str);
	}
}

void change_tan(int player){
	int player_offset = OFFSET_PLAYERS+SIZE_PLAYER*player;
	int menuindex = 0;
	int menucount = 16;

	char headerstr[] = "Select tan value";
	char* menu_entries[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;
		
		storeByte(gardenData, player_offset+OFFSET_PLAYER_TAN, (u8)menuindex);

		gfx_waitmessage("Changing tan value to %d... Done!", menuindex);
	}
}

void change_hair_style(int player){
	int i;
	int player_offset = OFFSET_PLAYERS+SIZE_PLAYER*player;
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

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		storeByte(gardenData, player_offset+OFFSET_PLAYER_HAIRSTYLE, (u8)menuindex);

		gfx_waitmessage("Changing hair style to %s... Done!", menu_entries[menuindex]);
	}
	for(i = 0; i < 34; i++)
		free(menu_entries[i]);
}

void change_hair_color(int player){
	int player_offset = OFFSET_PLAYERS+SIZE_PLAYER*player;
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

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		storeByte(gardenData, player_offset+OFFSET_PLAYER_HAIRCOLOR, (u8)menuindex);

		gfx_waitmessage("Changing hair color to %s... Done!", menu_entries[menuindex]);
	}
}

void change_eye_color(int player){
	int player_offset = OFFSET_PLAYERS+SIZE_PLAYER*player;
	int menuindex = 0;
	int menucount = 8;

	char headerstr[] = "Select a eye color";
	char* menu_entries[] = {"0", "1", "2", "3", "4", "5", "6", "7"};

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		storeByte(gardenData, player_offset+OFFSET_PLAYER_EYECOLOR, (u8)menuindex);

		gfx_waitmessage("Changing eye color to %d... Done!", menuindex);
	}
}

void change_face(int player){
	int player_offset = OFFSET_PLAYERS+SIZE_PLAYER*player;
	int menuindex = 0;
	int menucount = 12;

	char headerstr[] = "Select a face type";
	char* menu_entries[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"};

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		storeByte(gardenData, player_offset+OFFSET_PLAYER_FACE, (u8)menuindex);

		gfx_waitmessage("Changing face type to %d... Done!", menuindex);
	}
}


//villager select & villager menu
void villager_menu(int villager){
	int menuindex = 0;
	int menucount = 1;

	char headerstr[] = "Select an option";
	char* menu_entries[] = {
		"Toggle whether boxed",
		"Reset villager to defaults (not yet working!)",
		"Set villager (not yet working!)"
	};

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		switch(menuindex){
			case 0:
				toggle_boxed(villager);
				break;
			case 1:
				reset_villager(villager);
				break;
			case 2:
				set_villager(villager);
				break;
		}
	}
}

void box_menu(int villager){
	//legacy menu, here for future reference
	int menuindex = 0;
	int menucount;
	if(debug == 2)
		menucount = 3;
	else
		menucount = 2;

	char headerstr[] = "Select an option";
	char* menu_entries[] = {
		"Box villager",
		"Unbox villager",
		"Display status IDs"
	};

	while(aptMainLoop()){
		display_menu(menu_entries, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		switch(menuindex){
			case 0:
				//box_villager(villager);
				break;
			case 1:
				//unbox_villager(villager);
				break;
			case 2:
				display_status_id(villager);
				break;
		}
	}
}

void toggle_boxed(int villager){
	int villager_offset = OFFSET_VILLAGERS+SIZE_VILLAGER*villager;
	u32 status = readByte4(gardenData, villager_offset+OFFSET_VILLAGER_STATUS);
	char* str;
	
	if(status == (status | 0x00000001)){ //check if last byte is odd (boxed)
		str = "Unboxing";
		status = (status & ~0x00000001); //unbox villager
	}
	else{
		str = "Boxing";
		status = (status | 0x00000001); //box villager
	}

	storeByte(gardenData, villager_offset+OFFSET_VILLAGER_STATUS, (status & 0x000000ff));

	gfx_waitmessage("%s villager... Done!", str);
}

void reset_villager(int villager){
	//TODO
	int villager_offset = OFFSET_VILLAGERS+SIZE_VILLAGER*villager;
	int i;
	u16 itemid;
	u8 flag1;
	u8 flag2;

	gfx_displaymessage("Resetting villager to defaults...");
	set_ustring(gardenData, villager_offset+OFFSET_VILLAGER_CATCHPHRASE, villagers[villager].catchphrase, 11);
	//0-4 = shirt, song, wall, floor, umbrella. 5-21 furnature
	for(i = 0; i < 21; i++){
		itemid = villagers[villager].data[i] & 0xffff;
		flag1 = villagers[villager].data[i] >> 16 & 0xff;
		flag2 = villagers[villager].data[i] >> 24 & 0xff;
	
		storeByte2(gardenData, villager_offset+OFFSET_VILLAGER_SHIRT+4*i, itemid);
		storeByte(gardenData, villager_offset+OFFSET_VILLAGER_SHIRT+4*i+2, flag1);
		storeByte(gardenData, villager_offset+OFFSET_VILLAGER_SHIRT+4*i+3, flag2);
	}
	//unknown last 4 bytes
	storeByte4(gardenData, villager_offset+OFFSET_VILLAGER_SHIRT+4*21, villagers[villager].data[21]);
	gfx_waitmessage("Resetting villager to defaults... Done!");
}

void set_villager(int villager){
	//TODO
	u16 id = 0;
	int villager_offset = OFFSET_VILLAGERS+SIZE_VILLAGER*villager;
	id = id;
	villager_offset = villager_offset;
}


//misc menu
void unlock_all_pwps(){
	unsigned int i;

	if(gardenData == NULL){
		gfx_waitmessage("Please load a file first!");
		return;
	}
	u8 pwp_ids[]={
		0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		0xff,0xff,0xff,0xff
	};

	for(i = 0; i < sizeof(pwp_ids)/sizeof(u8); i++){
		storeByte(gardenData, OFFSET_TOWN_AVAILABLEPWPS+i, pwp_ids[i]);
	}
	
	gfx_waitmessage("Unlocking all PWPs... Done!");
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

	while(aptMainLoop()){
		menucount = sizeof(list)/sizeof(list[0]);

		display_menu(list, menucount, &menuindex, headerstr);

		if(menuindex == -1)
			break;

		gfx_waitmessage("menuindex: %d", menuindex);
	}

	for(i = 0; i < sizeof(list)/sizeof(list[0]); i++){
		free(list[i]);
	}
}

void view_apt_id(){
	u64 id;
	APT_GetProgramID(&id);

	if(id == 0x000400000cce2e00)
		sf2d_set_clear_color(RGBA8(0x00, 0xFF, 0x00, 0xFF)); //green
	else
		sf2d_set_clear_color(RGBA8(0xFF, 0x00, 0x00, 0xFF)); //red

	gfx_waitmessage("APT ID: %llx", id);
	
	sf2d_set_clear_color(RGBA8(0x00, 0x2e, 0x00, 0xFF));
}

void display_status_id(int villager){
	int villager_offset = OFFSET_VILLAGERS+SIZE_VILLAGER*villager;
	u32 status_id = readByte4(gardenData, villager_offset+OFFSET_VILLAGER_STATUS);
	gfx_waitmessage("Status ID: 0x%x", status_id);
}
