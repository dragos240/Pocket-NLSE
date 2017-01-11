#include <stdio.h>
#include <stdlib.h>

#include <sf2d.h>
#include <sfil.h>

#include "acres.h"

//acre textures
sf2d_texture* acre_textures[206];

void acres_init(){
	int i;
	char* basepath = "romfs:/acres_plus/";
	char* str = NULL;

	for(i = 0; i <= 205; i++){
		if((i <= 176) || (i >= 182 && i <=205)){
			str = calloc(strlen(basepath)+3+1+3+1+1, 1);
			snprintf(str, strlen(basepath)+3+1+3+1, "romfs:/acres_plus/%d.png", i);
			acre_textures[i] = sfil_load_PNG_file(str, SF2D_PLACE_RAM);
			free(str);
		}
		else
			acre_textures[i] = NULL;
	}
}

void acres_fini(){
	int i;

	for(i = 0; i <= 205; i++){
		if((i <= 176) || (i >= 182 && i <=205))
			sf2d_free_texture(acre_textures[i]);
	}
}
