#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <3ds.h>

#include "core/fs.h"
#include "core/conf.h"
#include "ui/error.h"
#include "ui/prompt.h"
#include "core/common.h"

FS_MediaType mediatype;
u64 titleid;
u32 lowerid, upperid, uniqueid;

FS_Archive game_arch, sdmc_arch;

Result fs_init(){
	Handle fs_handle;
	Result ret;
	fsInit();
	ret = srvGetServiceHandleDirect(&fs_handle, "fs:USER");
	if(ret)
		return ret;
	ret = FSUSER_Initialize(fs_handle);
	if(ret)
		return ret;
	fsUseSession(fs_handle);

	return ret;
}

Result open_archives(){
	Result ret;
	u32* path;
	FS_Path null_binpath, game_binpath;

	//open sdmc card
	null_binpath = (FS_Path){PATH_EMPTY, 1, (u8*)""};
	ret = FSUSER_OpenArchive(&sdmc_arch, ARCHIVE_SDMC, null_binpath);
	if(ret)
		return ret;
	FSUSER_CreateDirectory(sdmc_arch, fsMakePath(PATH_ASCII, "/Pocket-NLSE"), 0);
	FSUSER_CreateDirectory(sdmc_arch, fsMakePath(PATH_ASCII, "/Pocket-NLSE/Saves"), 0);

	//open game archive
	path = (u32[3]){mediatype, lowerid, upperid};
	game_binpath = (FS_Path){PATH_BINARY, 12, path};
	ret = FSUSER_OpenArchive(&game_arch, ARCHIVE_USER_SAVEDATA, game_binpath);
	if(ret)
		return ret;

	return ret;
}
Result open_sdmc_archive(){
	Result ret;
	FS_Path null_binpath;

	null_binpath = (FS_Path){PATH_EMPTY, 1, (u8*)""};
	ret = FSUSER_OpenArchive(&sdmc_arch, ARCHIVE_SDMC, null_binpath);
	if(ret)
		return ret;
	FSUSER_CreateDirectory(sdmc_arch, fsMakePath(PATH_ASCII, "/Pocket-NLSE"), 0);
	FSUSER_CreateDirectory(sdmc_arch, fsMakePath(PATH_ASCII, "/Pocket-NLSE/Saves"), 0);

	return ret;
}

Result open_game_archive(){
	Result ret;
	u32* path;
	FS_Path game_binpath;

	path = (u32[3]){mediatype, lowerid, upperid};
	game_binpath = (FS_Path){PATH_BINARY, 12, path};
	ret = FSUSER_OpenArchive(&game_arch, ARCHIVE_USER_SAVEDATA, game_binpath);
	if(ret)
		return ret;

	return ret;
}

Result close_archives(){
	Result ret;
	
	ret = FSUSER_CloseArchive(game_arch);
	ret |= FSUSER_CloseArchive(sdmc_arch);

	return ret;
}

void fs_fini(){
	fsEndUseSession();
	fsExit();
}

int load_config(){
	//loads config file and sets *current_town
	Result ret;
	Handle handle;
	FILE* f;
	u64 size;
	int tempsize;
	u32 read; //bytes read
	u32 written; //bytes written
	char* buf;
	conftok_t token;
	const char* configpath = "/Pocket-NLSE/config";

	//if config file was just created now
	if((ret = FSUSER_CreateFile(sdmc_arch, fsMakePath(PATH_ASCII, configpath), 0, 0)) == 0){
		//open template config file
		if(!(f = fopen("romfs:/config", "r"))){
			error_display(NULL, NULL, "Could not open template config file.");
			return -1;
		}
		fseek(f, 0L, SEEK_END);
		tempsize = ftell(f);
		rewind(f);
		if((buf = malloc(tempsize)) == NULL){
			error_display(NULL, NULL, "malloc failed.");
			return -1;
		}
		fread(buf, tempsize, 1, f);
		fclose(f);

		//open the newly created config file
		if((ret = FSUSER_OpenFile(&handle, sdmc_arch, fsMakePath(PATH_ASCII, configpath), FS_OPEN_READ | FS_OPEN_WRITE, 0))){
			error_display_res(NULL, NULL, ret, "Failed to open config file");
			return -1;
		}
		conf_parse(buf, &token);
		get_mediatype();
		token.mediatype_val = mediatype;
		free(buf);
		conf_gen(&buf, &token);
		size = strlen(buf);
		if((ret = FSFILE_Write(handle, &written, 0, buf, size, FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME))){
			error_display_res(NULL, NULL, ret, "Failed to write config file");
			return -1;
		}
		if(size != written){
			error_display(NULL, NULL, "size != written.");
			return -1;
		}
		if((ret = FSFILE_SetSize(handle, size))){
			error_display_res(NULL, NULL, ret, "Failed to set size of config file");
			return -1;
		}
		if((ret = FSFILE_Close(handle))){
			error_display_res(NULL, NULL, ret, "Failed to close config file");
			return -1;
		}
		return 0;
	}
	//if config file exists
	else{
		if((ret = FSUSER_OpenFile(&handle, sdmc_arch, fsMakePath(PATH_ASCII, configpath), FS_OPEN_READ, 0))){
			error_display_res(NULL, NULL, ret, "Failed to open config file");
			return -1;
		}
		if((ret = FSFILE_GetSize(handle, &size))){
			error_display_res(NULL, NULL, ret, "Failed to get size of config file");
			return -1;
		}
		if((buf = malloc(size)) == NULL){
			error_display(NULL, NULL, "malloc failed.");
			return -1;
		}
		if((ret = FSFILE_Read(handle, &read, 0, buf, size))){
			error_display_res(NULL, NULL, ret, "Failed to read config file");
			return -1;
		}
		if((ret = FSFILE_Close(handle))){
			error_display_res(NULL, NULL, ret, "Failed to close config file");
			return -1;
		}
		conf_parse(buf, &token);
		mediatype = token.mediatype_val;
	}

	return 0;
}

int get_mediatype(){
	int menuindex = 0;
	int menucount = 2;

	char headerstr[] = "Select the mediatype of your game.";

	char* menu_entries[] = {
		"eShop/CIA version",
		"Cartridge"
	};

	//display_menu(menu_entries, menucount, &menuindex, headerstr);
	if(menuindex == -1)
		return -1;

	mediatype = menuindex+1;

	return 0;
}

void change_mediatype(){
	Result ret;
	Handle handle;
	u32 read;
	u32 written;
	u32 size;
	char* buf;
	conftok_t token;

	get_mediatype();
	if((ret = FSUSER_OpenFile(&handle, sdmc_arch, fsMakePath(PATH_ASCII, "/Pocket-NLSE/config"), FS_OPEN_READ | FS_OPEN_WRITE, 0))){
		error_display_res(NULL, NULL, ret, "Failed to open config file");
		return;
	}
	if((ret = FSFILE_GetSize(handle, (u64*)&size))){
		error_display_res(NULL, NULL, ret, "Failed to get size of config file");
		return;
	}
	buf = malloc(size);
	if((ret = FSFILE_Read(handle, &read, 0, buf, size))){
		error_display_res(NULL, NULL, ret, "Failed to read config file");
		return;
	}
	conf_parse(buf, &token);
	token.mediatype_val = mediatype;
	conf_gen(&buf, &token);
	if((ret = FSFILE_Write(handle, &written, 0, buf, size, FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME))){
		error_display_res(NULL, NULL, ret, "Failed to write config file");
		return;
	}
	if((ret = FSFILE_Close(handle))){
		error_display_res(NULL, NULL, ret, "Failed to close config file");
		return;
	}

	prompt_display("Success", "Changed media source.", COLOR_BLACK, false, NULL, NULL, NULL);
}

int get_titleid(){
	Result ret;
	u32 num_sdtitles;
	u64* sd_titles;
	int i;

	amInit();
	if(mediatype == MEDIATYPE_GAME_CARD){
		AM_GetTitleList(NULL, mediatype, 1, &titleid);
	}
	else{
		ret = AM_GetTitleCount(mediatype, &num_sdtitles);
		if(ret){
			error_display_res(NULL, NULL, ret, "AM_GetTitleCount failed.");
			return -1;
		}
		sd_titles = malloc(num_sdtitles*sizeof(u64));
		ret = AM_GetTitleList(NULL, mediatype, num_sdtitles, sd_titles);
		if(ret){
			error_display_res(NULL, NULL, ret, "AM_GetTitleList failed.");
			return -1;
		}
		titleid = 1337LL;
		for(i = 0; i < (signed)num_sdtitles && titleid == 1337LL; i++){
			if(sd_titles[i] == NA_GAMEID)
				titleid = NA_GAMEID;
			else if(sd_titles[i] == EU_GAMEID)
				titleid = EU_GAMEID;
			else if(sd_titles[i] == JP_GAMEID)
				titleid = JP_GAMEID;
		}
		if(titleid == 1337LL){
			error_display(NULL, NULL, "Could not find a copy of AC:NL on this device.");
			return -1;
		}
	}
	amExit();
	if(titleid != NA_GAMEID && titleid != EU_GAMEID && titleid != JP_GAMEID){
		char buf[256];
		sprintf(buf, "Unknown titleID 0x%llX", titleid);
		error_display(NULL, NULL, buf);
		return -1;
	}
	lowerid = (u32)(titleid);
	upperid = (u32)(titleid >> 32);
	uniqueid = (lowerid >> 8);
	
	return 0;
}
