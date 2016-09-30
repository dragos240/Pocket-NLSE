#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sf2d.h>
#include <sftd.h>

#include "menu.h"
#include "gfx.h"
#include "ui.h"
#include "dir.h"

static int fontheight = 11;
//static int fontwidth = 7;

char* browse_dir(char* headerstr){
	int i;
	int dirsize;
	int dirlevel = 0;
	char* filename;
	char* curdir = NULL,* prevdir[20];
	char* path = NULL;
	char** filenames;
	DIR* dir;
	struct dirent* ent;
	struct stat path_stat;

	int menuindex = 0;
	prevdir[0] = "/";

	curdir = calloc(2, 1);curdir[0]='/';
	path = calloc(2, 1);path[0]='/';

	while(1){
		char str[500];
		snprintf(str, 500, "%s - %s", headerstr, curdir);
		menuindex = 0;
		dirsize = 2; //. and ..
		dir = opendir(curdir);
		//get size of all files/folders
		while((ent = readdir(dir)) != NULL){
			sf2d_start_frame(GFX_TOP, GFX_LEFT);
				ui_frame();
				sftd_draw_text(font, 0, fontheight*2, COLOR_WHITE, fontheight, "Calculating number of files/dirs...");
			sf2d_end_frame();
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_end_frame();
			sf2d_swapbuffers();
			dirsize++;
		}
		closedir(dir);

		filenames = malloc(sizeof(char*)*dirsize+1);
		filenames[0] = calloc(3, 1);
		strcpy(filenames[0], "./");
		filenames[1] = calloc(4, 1);
		strcpy(filenames[1], "../");

		dir = opendir(curdir);
		i = 2;
		while((ent = readdir(dir)) != NULL){
			filename = ent->d_name;
			filenames[i] = calloc(strlen(filename)+2, 1); //leave extra room for the null terminator and if it's a directory, a frontslash
			strcpy(filenames[i], filename);
			free(path); //make sure we're not leaking memory
			path = calloc(strlen(curdir)+strlen(filename)+1, 1);
			strcpy(path, curdir);
			strcat(path, filename);
			stat(path, &path_stat); //stat path to check to see if it's a file or directory
			if(S_ISDIR(path_stat.st_mode)){
				filenames[i][strlen(filenames[i])] = '/'; //put a '/' at the end of the name if it's a directory
			}
			i++;
		}
		closedir(dir);

		display_menu(filenames, dirsize, &menuindex, str);
		if(menuindex==-1){
			return "";
		}

		if(menuindex == 0){ //if selection is .
			return curdir;
		}
		else if(menuindex == 1){ //if selection is ..
			if(dirlevel != 0){
				dirlevel--;
				curdir = prevdir[dirlevel];
			}
		}
		else if(filenames[menuindex][strlen(filenames[menuindex])-1] == '/'){ //if selection is dir
			char* tmpstr;
			tmpstr = curdir;
			prevdir[dirlevel] = curdir;
			dirlevel++;
			curdir = calloc(strlen(curdir) + strlen(filenames[menuindex]) + 1, 1);
			strcpy(curdir, tmpstr);
			strcat(curdir, filenames[menuindex]);
		}
		else{ //if selection is file
			free(path);
			path = calloc(strlen(curdir)+strlen(filenames[menuindex])+1, 1);
			strcpy(path, curdir);
			strcat(path, filenames[menuindex]);
			return path;
		}
	}
}
