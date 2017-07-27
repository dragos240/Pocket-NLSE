#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <3ds.h>

#include "core/http.h"
#include "core/fs.h"
#include "ui/prompt.h"
#include "core/common.h"
#include "core/screen.h"
#include "ui/error.h"
#include "actions/simple.h"

static bool acres_initialized;

bool acres_download(){
	Result ret;
	Handle handle;
	int i;
	char* basepath = "/Pocket-NLSE/acres_plus/";
	char* baseurl = "http://osakasystem.us/Pocket-NLSE/acres_plus/";
	char* path;
	char* url;
	u32 written;
	int size;

	httpcInit(0);
	FSUSER_CreateDirectory(sdmc_arch, fsMakePath(PATH_ASCII, "/Pocket-NLSE/acres_plus"), 0);

	for(i = 0; i <= 205; i++){
		if((i <= 176) || (i >= 182 && i <=205)){
			//TODO insert loading bar here
			size = strlen(baseurl)+7+1;
			url = calloc(size, 1);
			snprintf(url, size, "%s%d.png", baseurl, i);
			ret = http_download(url);
			size = strlen(basepath)+7+1;
			path = calloc(size, 1);
			snprintf(path, size, "%s%d.png", basepath, i);
			if((ret = FSUSER_OpenFile(&handle, sdmc_arch, fsMakePath(PATH_ASCII, path), FS_OPEN_CREATE | FS_OPEN_WRITE, 0)) != 0){
				error_display_res(NULL, NULL, ret, "Failed to open PNG file.");
				return false;
			}
			if((FSFILE_Write(handle, &written, 0, imgdata, imgsize, FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME)) != 0){
				error_display_res(NULL, NULL, ret, "Failed to write PNG file.");
				return false;
			}
			FSFILE_Close(handle);
			free(url);
			free(path);
		}
	}
	prompt_display("Success", "Acres download finished.", COLOR_BLACK, false, NULL, NULL, NULL);

	httpcExit();

	return true;
}

void acres_init(){
	int i;
	char* basepath = "/Pocket-NLSE/acres_plus/";
	char* str = NULL;

	if(acres_initialized == true)
		return;

	//check to see if files exist already
	if(access("/Pocket-NLSE/acres_plus/205.png", F_OK) == -1)
		if(acres_download() == false)
			return;

	for(i = 0; i <= 205; i++){
		//insert loading bar here TODO
		if((i <= 176) || (i >= 182 && i <=205)){
			str = calloc(strlen(basepath)+3+1+3+1+1, 1);
			snprintf(str, strlen(basepath)+3+1+3+1, "/Pocket-NLSE/acres_plus/%d.png", i);
			screen_load_texture_file(7+i, str, true);
			free(str);
		}
	}
	acres_initialized = true;
}

void populate_acres(u8** acres){
	*acres = malloc(7*6);
	int i, j;

	acres_init();
	//get acre bytes into acres
	j = 0;
	for(i = 0; i < 7*6*2; i++){
		if(i == 7*6*2-1){
			(*acres)[j] = readByte1(gardenData, OFFSET_MAP_ACRES+i-1);
			j++;
		}
		else if(i % 2 == 0){
			(*acres)[j] = readByte1(gardenData, OFFSET_MAP_ACRES+i-2);
			j++;
		}
	}
}
