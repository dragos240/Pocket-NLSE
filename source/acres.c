#include <stdio.h>
#include <stdlib.h>

#include <sf2d.h>
#include <sfil.h>

#include "acres.h"

//acre textures
sf2d_texture* acre_textures[213];
sf2d_texture* acre_texture_unk_157b;

void acres_init(){
	int i;
	char* str = NULL;

	for(i = 0; i <= 212; i++){
		if((i <= 174) || (i >= 180 && i <= 203) || (i >= 210 && i <=212)){
			str = calloc(21, 1);
			snprintf(str, 21, "romfs:/acres/%d.png", i);
			acre_textures[i] = sfil_load_PNG_file(str, SF2D_PLACE_RAM);
			free(str);
		}
		else
			acre_textures[i] = NULL;
	}
}

void acres_fini(){
	int i;

	for(i = 0; i <= 212; i++){
		if((i <= 174) || (i >= 180 && i <= 203) || (i >= 210 && i <=212))
			sf2d_free_texture(acre_textures[i]);
	}
}
