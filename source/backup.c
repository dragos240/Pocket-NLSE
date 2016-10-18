#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>

#include "common.h"
#include "gfx.h"

FS_Archive game_arch, sdmc_arch;

Result backup_init(){
	Handle fs_handle;
	Result ret;
	u64 titleid;
	u32 lowerid, upperid;
	u32* path;
	FS_Path null_binpath, game_binpath;

	if(is3dsx){
		fsInit();
		ret = srvGetServiceHandleDirect(&fs_handle, "fs:USER");
		if(ret)
			return ret;
		ret = FSUSER_Initialize(fs_handle);
		if(ret)
			return ret;

		fsUseSession(fs_handle);
	}
	
	//open sdmc card
	null_binpath = (FS_Path){PATH_EMPTY, 1, (u8*)""};
	ret = FSUSER_OpenArchive(&sdmc_arch, ARCHIVE_SDMC, null_binpath);
	if(ret)
		return ret;

	amInit();
	titleid = 0x0004000000086300;
	lowerid = (u32)(titleid);
	upperid = (u32)(titleid >> 32);
	if(is3dsx){
		ret = FSUSER_OpenArchive(&game_arch, ARCHIVE_SAVEDATA, fsMakePath(PATH_EMPTY, ""));
		if(ret)
			return ret;
	}
	else{
		path = (u32[3]){2, lowerid, upperid};
		game_binpath = (FS_Path){PATH_BINARY, 12, path};
		ret = FSUSER_OpenArchive(&game_arch, ARCHIVE_USER_SAVEDATA, game_binpath);
		if(ret)
			return ret;
	}

	return ret;
}

Result backup_fini(){
	Result ret;

	ret = FSUSER_CloseArchive(game_arch);
	ret |= FSUSER_CloseArchive(sdmc_arch);

	if(is3dsx){
		fsEndUseSession();
		fsExit();
	}
	amExit();

	return ret;
}

char* game_to_buffer(){
	u32 read; //bytes read
	u64 game_file_size;
	char* buffer = NULL;
	Result ret;
	Handle game_file_handle;

	//open garden.dat
	ret = FSUSER_OpenFile(&game_file_handle, game_arch, fsMakePath(PATH_ASCII, "/garden.dat"), FS_OPEN_READ, 0);
	if(ret){
	}

	//get size of garden.dat
	FSFILE_GetSize(game_file_handle, &game_file_size);
	if(game_file_size != GARDEN_SIZE){
		gfx_waitmessage("Corrupt save file detected! Aborting...");
		goto close_files;
	}

	//allocate space for garden.dat in buffer
	buffer = malloc(game_file_size+1);
	if(!buffer){
		gfx_waitmessage("malloc failed!");
		goto close_files;
	}

	//read garden.dat into buffer
	ret = FSFILE_Read(game_file_handle, &read, 0, buffer, game_file_size);
	if(ret){
		gfx_error(ret, __LINE__);
		goto close_files;
	}
close_files:

	ret = FSFILE_Close(game_file_handle);
	if(ret)
		gfx_error(ret, __LINE__);
	if(read == GARDEN_SIZE)
		return buffer;
	else
		return "";
}

void buffer_to_game(char* buffer){
	u32 written; //bytes written
	Result ret;
	Handle game_file_handle;
	
	//Writing
	ret = FSUSER_OpenFile(&game_file_handle, game_arch, fsMakePath(PATH_ASCII, "/garden.dat"), FS_OPEN_WRITE, 0);
	if(ret){
		gfx_error(ret, __LINE__);
		goto close_files;
	}

	ret = FSFILE_Write(game_file_handle, &written, 0, buffer, GARDEN_SIZE, FS_WRITE_FLUSH);
	if(ret){
		gfx_error(ret, __LINE__);
		goto close_files;
	}
	if(written != GARDEN_SIZE){
		gfx_waitmessage("cartridge's garden.dat incorrectly written to!");
		goto close_files;
	}

	close_files:

	ret = FSFILE_Close(game_file_handle);
	if(ret)
		gfx_error(ret, __LINE__);

	ret = FSUSER_ControlArchive(game_arch, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
	if(ret)
		gfx_error(ret, __LINE__);
}

void buffer_to_dir(char* buffer, char* dirname){
	u32 written; //bytes written
	char* path_out;
	char* file_out;
	Result ret;
	Handle sdmc_file_handle;

	path_out = calloc(100, 1);
	strcat(path_out, "/Pocket-NLSE/Saves/");
	strcat(path_out, dirname);
	strcat(path_out, "/");
	//attempt to create directory "/Pocket-NLSE/Saves/" if not able,
	//create parent directories until it works
	ret = FSUSER_CreateDirectory(sdmc_arch, fsMakePath(PATH_ASCII, path_out), 0);
	if(ret){
		if(ret == (Result)0xc8804478){
			ret = FSUSER_CreateDirectory(sdmc_arch, fsMakePath(PATH_ASCII, "/Pocket-NLSE/Saves/"), 0);
			if(ret == (Result)0xc8804478){
				ret = FSUSER_CreateDirectory(sdmc_arch, fsMakePath(PATH_ASCII, "/Pocket-NLSE/"), 0);
			}
			FSUSER_CreateDirectory(sdmc_arch, fsMakePath(PATH_ASCII, path_out), 0);
		}
	}

	//Writing
	file_out = calloc(strlen(path_out)+11, 1); //11 = garden.dat + '\0'
	sprintf(file_out, "%sgarden.dat", path_out);

	ret = FSUSER_OpenFile(&sdmc_file_handle, sdmc_arch, fsMakePath(PATH_ASCII, file_out), FS_OPEN_WRITE | FS_OPEN_CREATE, 0);
	if(ret){
	  gfx_error(ret, __LINE__);
	  goto close_files;
	}

	ret = FSFILE_Write(sdmc_file_handle, &written, 0, buffer, GARDEN_SIZE, FS_WRITE_FLUSH);
	if(ret){
		gfx_error(ret, __LINE__);
		goto close_files;
	}
	if(written != GARDEN_SIZE){
		gfx_waitmessage("local garden.dat incorrectly written to!");
		goto close_files;
	}

close_files:

	ret = FSFILE_Close(sdmc_file_handle);
	if(ret)
		gfx_error(ret, __LINE__);
}

char* dir_to_buffer(char* dirname){
	u32 read; //bytes read/written
	u64 sdmc_file_size;
	char* path_in;
	char* file_in;
	char* buffer = NULL;
	Result ret;
	Handle sdmc_file_handle;
	
	path_in = calloc(100, 1);
	strcat(path_in, "/Pocket-NLSE/Saves/");
	strcat(path_in, dirname);
	strcat(path_in, "/");
	
	file_in = calloc(strlen(path_in)+11, 1); //11 = garden.dat + '\0'
	sprintf(file_in, "%sgarden.dat", path_in);

	//open garden.dat
	ret = FSUSER_OpenFile(&sdmc_file_handle, sdmc_arch, fsMakePath(PATH_ASCII, file_in), FS_OPEN_READ, 0);
	if(ret){
		gfx_error(ret, __LINE__);
		goto close_files;
	}

	//get size of garden.dat
	FSFILE_GetSize(sdmc_file_handle, &sdmc_file_size);

	//allocate space for garden.dat in buffer
	buffer = malloc(sdmc_file_size+1);
	if(!buffer){
		gfx_waitmessage("malloc failed!");
		goto close_files;
	}

	//read garden.dat into buffer
	ret = FSFILE_Read(sdmc_file_handle, &read, 0, buffer, sdmc_file_size);
	if(ret){
		gfx_error(ret, __LINE__);
		goto close_files;
	}
	if(read != GARDEN_SIZE){
		gfx_waitmessage("Problem reading from file!");
		buffer = "";
		goto close_files;
	}

	close_files:

	ret = FSFILE_Close(sdmc_file_handle);
	if(ret)
		gfx_error(ret, __LINE__);
	
	return buffer;
}
