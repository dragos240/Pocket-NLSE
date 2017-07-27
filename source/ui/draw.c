#include <3ds.h>

#include "core/screen.h"
#include "ui/draw.h"
#include "actions/simple.h"
#include "core/constants.h"

void draw_pockets(int player){
	int i, j;
	const int item_w = 50;
	const int item_h = 41;
	u32 kDown;
	u32 kHeld;
	touchPosition touch;

	while(aptMainLoop()){
		hidScanInput();
		kDown = hidKeysDown();
		kHeld = hidKeysHeld();

		if(kDown & KEY_B)
			break;

		screen_begin_frame();
		screen_select(GFX_BOTTOM);
			screen_draw_texture(TEXTURE_POCKETS, 0, 0, 320, 140);
			//touchpad controls
			if(kHeld & KEY_TOUCH){
				hidTouchRead(&touch);
				if(touch.px >= 16 && touch.px <= 16+item_w*6 &&
					touch.py >= 3 && touch.py <= 3+item_h*3){
					//1st row
					if(touch.py >= 3 && touch.py <= 3+item_h){
						//1st col
						if(touch.px >= 16 && touch.px <= 16+37){
							//sf2d_draw_
						}
						//2nd col
						else if(touch.px >= 16+item_w && touch.px <= 16+item_w+37){
						}
						//3rd col
						else if(touch.px >= 16+item_w*2 && touch.px <= 16+item_w*2+37){
						}
						//4th col
						else if(touch.px >= 16+item_w*3 && touch.px <= 16+item_w*3+37){
						}
						//5th col
						else if(touch.px >= 16+item_w*4 && touch.px <= 16+item_w*4+37){
						}
						//6th col
						else if(touch.px >= 16+item_w*5 && touch.px <= 16+item_w*5+37){
						}
					}
					//2nd row
					else if(touch.py >= 3+item_h && touch.py <= 3+item_h*2){
						
					}
					//3rd row
					else if(touch.py >= 3+item_h*2 && touch.py <= 3+item_h*3){
						
					}
				}
			}
			for(i = 0; i < 3; i++){
				for(j = 0; j < 6; j++){
					//if item >16 break
					if(i == 2 && j == 4)
						break;
					if(pockets[player][j+i*6].itemid != ITEM_EMPTY){
						screen_draw_texture(TEXTURE_ITEM_OCCUPIED, 16+item_w*j, 3+item_h*i, 37, 37);
					}
				}
			}
		screen_end_frame();
	}
}

void draw_map(mapctrl_t* mapctrl, u8* acres){
	//will be removed later
	mapctrl = mapctrl;
	acres = acres;
	/*
	int i, j;
	//top screen (acres)
	screen_begin_frame();
	screen_select(GFX_TOP);
		ui_frame("Use dPad to select acre. Touch tiles to change item");
		screen_draw_texture(TEXTURE_MAP_BACK, 85, 33, 229, 185);
		
		//draw acres
		for(i = 0; i < 6; i++){
			for(j = 0; j < 7; j++){
				if(i == 0 && (acre_textures[acres[(i*7)+(j+1)]] != NULL)) //first row
					screen_draw_texture_
					sf2d_draw_texture_scale(acre_textures[acres[(i*7)+(j+1)]], 88+j*32, 36, 0.49, 0.49);
				else if(acre_textures[acres[(i*7)+(j+1)]] != NULL) //all other rows
					sf2d_draw_texture_scale(acre_textures[acres[(i*7)+(j+1)]], 88+j*32, 24+i*32, 0.49, 0.49);
			}
		}
		//draw acre cursor overlay
		if(mapctrl->acre_pos[1] == 0)
			sf2d_draw_rectangle(88+32*(mapctrl->acre_pos[0]), 36, 31, 19, RGBA8(0x00, 0x00, 0xFF, 0x088));
		else
			sf2d_draw_rectangle(88+32*(mapctrl->acre_pos[0]), 24+32*(mapctrl->acre_pos[1]), 31, 31, RGBA8(0x00, 0x00, 0xFF, 0x88));
		mapctrl->acre_type = ((mapctrl->acre_pos[1])*7)+(mapctrl->acre_pos[0]+1);
	
	//bottom screen (tiles)
	screen_select(GFX_BOTTOM);
		sf2d_draw_rectangle(56-3, 15-3, 208+6, 208+6, COLOR_BLACK);
		sf2d_draw_rectangle(56, 15, 208, 208, RGBA8(0x77, 0x77, 0x77, 0xFF));
		for(i = 0; i < 16; i++){ //y
			for(j = 0; j < 16; j++){ //x
				sf2d_draw_texture_part_scale(acre_textures[acres[mapctrl->acre_type]], 56+j*13, 15+i*13, j*4, i*4, 4, 4, 3.00, 3.00);
			}
		}
	screen_end_frame();
	*/
}
