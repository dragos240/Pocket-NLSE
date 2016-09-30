#include <stdlib.h>

#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>

#include "gfx.h"
#include "ui.h"
#include "kb.h"

static char* kb_lower_chars[5][10] = {
								{"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
								{"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
								{"a", "s", "d", "f", "g", "h", "j", "k", "l", "-"},
								{"z", "x", "c", "v", "b", "n", "m", ".", "Bksp", ""},
								{"Shift", "", "", "Space", "", "", "", "", "OK", ""}
						   	  };

static char* kb_upper_chars[5][10] = {
								{"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
								{"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"},
								{"A", "S", "D", "F", "G", "H", "J", "K", "L", "_"},
								{"Z", "X", "C", "V", "B", "N", "M", ".", "Bksp", ""},
								{"Shift", "", "", "Space", "", "", "", "", "OK", ""}
							  };

static kbkey_t keys[5][10];
static int KEY_HEIGHT = 22;
static int KEY_WIDTH = 32;
static int kb_y_offset = 130;

static int fontheight = 11;
//static int fontwidth = 7;

static sf2d_texture *key;
static sf2d_texture *keydown;
static sf2d_texture *key2;
static sf2d_texture *key2down;
static sf2d_texture *key4;
static sf2d_texture *key4down;

void kb_init(){
	int i, j;

	key = sfil_load_PNG_file("romfs:/key.png", SF2D_PLACE_RAM);
	keydown = sfil_load_PNG_file("romfs:/keydown.png", SF2D_PLACE_RAM);
	key2 = sfil_load_PNG_file("romfs:/key2.png", SF2D_PLACE_RAM);
	key2down = sfil_load_PNG_file("romfs:/key2down.png", SF2D_PLACE_RAM);
	key4 = sfil_load_PNG_file("romfs:/key4.png", SF2D_PLACE_RAM);
	key4down = sfil_load_PNG_file("romfs:/key4down.png", SF2D_PLACE_RAM);
	
	for(i = 0; i < 5; i++){
		for(j = 0; j < 10; j++){
			if(i == 4 && j == 3){ //space (4)
				keys[i][j].start_x = j*KEY_WIDTH;
				keys[i][j].end_x = j*KEY_WIDTH+KEY_WIDTH*4;
				keys[i][j].start_y = kb_y_offset+i*KEY_HEIGHT;
				keys[i][j].end_y = kb_y_offset+(i+1)*KEY_HEIGHT;
				keys[i][j].contents = kb_lower_chars[i][j];
			}
			else if(i == 3 && j == 8){ //backspace (2)
				keys[i][j].start_x = j*KEY_WIDTH;
				keys[i][j].end_x = j*KEY_WIDTH+KEY_WIDTH*2;
				keys[i][j].start_y = kb_y_offset+i*KEY_HEIGHT;
				keys[i][j].end_y = kb_y_offset+(i+1)*KEY_HEIGHT;
				keys[i][j].contents = kb_lower_chars[i][j];
			}
			else if(i == 4 && j == 0){ //shift (2)
				keys[i][j].start_x = j*KEY_WIDTH;
				keys[i][j].end_x = j*KEY_WIDTH+KEY_WIDTH*2;
				keys[i][j].start_y = kb_y_offset+i*KEY_HEIGHT;
				keys[i][j].end_y = kb_y_offset+(i+1)*KEY_HEIGHT;
				keys[i][j].contents = kb_lower_chars[i][j];
			}
			else if(i == 4 && j == 8){ //OK (2)
				keys[i][j].start_x = j*KEY_WIDTH;
				keys[i][j].end_x = j*KEY_WIDTH+KEY_WIDTH*2;
				keys[i][j].start_y = kb_y_offset+i*KEY_HEIGHT;
				keys[i][j].end_y = kb_y_offset+(i+1)*KEY_HEIGHT;
				keys[i][j].contents = kb_lower_chars[i][j];
			}
			else if(i < 3 || (i == 3 && j < 8)){ //characters (1)
				keys[i][j].start_x = j*KEY_WIDTH;
				keys[i][j].end_x = (j+1)*KEY_WIDTH;
				keys[i][j].start_y = kb_y_offset+i*KEY_HEIGHT;
				keys[i][j].end_y = kb_y_offset+(i+1)*KEY_HEIGHT;
				keys[i][j].contents = kb_lower_chars[i][j];
			}
			else{
				keys[i][j].start_x = -1;
				keys[i][j].end_x = -1;
				keys[i][j].start_y = -1;
				keys[i][j].end_y =-1;
				keys[i][j].contents = "";
			}
		}
	}
}

void kb_fini(){
	sf2d_free_texture(key);
	sf2d_free_texture(keydown);
	sf2d_free_texture(key2);
	sf2d_free_texture(key2down);
	sf2d_free_texture(key4);
	sf2d_free_texture(key4down);
}

char* draw_kb(char* str){
	int i, j, pos = 0;

	char* outstr = calloc(256, 1);

	u16 touch_x = 320/2;
	u16 touch_y = 240/2;
	touchPosition touch;

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		u32 kUp = hidKeysUp();
		if (kDown & KEY_B)
			break;
		else if(kDown & KEY_TOUCH){
			hidTouchRead(&touch);
			touch_x = touch.px;
			touch_y = touch.py;
		}

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			ui_frame();
			if(str == NULL){
				sftd_draw_text(font_bold, 1, 1, COLOR_BLACK, fontheight, "Enter your text");
				sftd_draw_text(font_bold, 0, 0, COLOR_WHITE, fontheight, "Enter your text");
			}
			else{
				sftd_draw_textf(font_bold, 1, 1, COLOR_BLACK, fontheight, "%s", str);
				sftd_draw_textf(font_bold, 0, 0, COLOR_WHITE, fontheight, "%s", str);
			}
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			kb_frame();
			sftd_draw_textf(font, fontheight*2, fontheight*2, COLOR_WHITE, fontheight, "%s", outstr);
			for(i = 0; i < 5; i++){
				for(j = 0; j < 10; j++){
					//draw kb
					//character pressed
					if(touch_x > keys[i][j].start_x && touch_x < keys[i][j].end_x &&
							touch_y > keys[i][j].start_y && touch_y < keys[i][j].end_y &&
							(i <= 2 || (i == 3 && j < 8)) && (kHeld & KEY_TOUCH)){
						sf2d_draw_texture(keydown, keys[i][j].start_x, keys[i][j].start_y);
					}
					//character unpressed
					else if(i <= 2 || (i == 3 && j < 8)){
						sf2d_draw_texture(key, keys[i][j].start_x, keys[i][j].start_y);
					}
					//character (2) pressed
					else if(touch_x > keys[i][j].start_x && touch_x < keys[i][j].end_x &&
							touch_y > keys[i][j].start_y && touch_y < keys[i][j].end_y &&
							((i == 4 && j == 0) || (i == 4 && j == 8) || (i == 3 && j == 8)) && 
							(kHeld & KEY_TOUCH)){
						sf2d_draw_texture(key2down, keys[i][j].start_x, keys[i][j].start_y);
					}
					//character (2) unpressed
					else if((i == 4 && j == 0) || (i == 4 && j == 8) || (i == 3 && j == 8)){
						sf2d_draw_texture(key2, keys[i][j].start_x, keys[i][j].start_y);
					}
					//character (4) pressed
					if(touch_x > keys[i][j].start_x && touch_x < keys[i][j].end_x &&
							touch_y > keys[i][j].start_y && touch_y < keys[i][j].end_y &&
							(i == 4 && j == 3) && (kHeld & KEY_TOUCH)){
						sf2d_draw_texture(key4down, keys[i][j].start_x, keys[i][j].start_y);
					}
					//character (4) unpressed
					else if(i == 4 && j == 3){
						sf2d_draw_texture(key4, keys[i][j].start_x, keys[i][j].start_y);
					}
					
					//text input
					//if alphanumeric, "."/"," or "-"/"_" character
					if(touch_x > keys[i][j].start_x && touch_x < keys[i][j].end_x &&
							touch_y > keys[i][j].start_y && touch_y < keys[i][j].end_y &&
							pos != 255 && (kUp & KEY_TOUCH) && (i < 3 || (i == 3 && j < 8))){
						outstr[pos] = keys[i][j].contents[0];
						pos++;
					}
					//if OK
					else if(touch_x > keys[4][8].start_x && touch_x < keys[4][8].end_x &&
							touch_y > keys[4][8].start_y && touch_y < keys[4][8].end_y &&
							(kDown & KEY_TOUCH)){
						sf2d_end_frame();
						sf2d_swapbuffers();
						return outstr;
					}
					//if backspace
					else if(touch_x > keys[3][8].start_x && touch_x < keys[3][8].end_x &&
							touch_y > keys[3][8].start_y && touch_y < keys[3][8].end_y &&
							pos != 255 && pos != 0 && (kUp & KEY_TOUCH)){
						pos--;
						touch_x = -1; touch_y = -1;
						outstr[pos] = '\0';
					}
					//if space
					else if(touch_x > keys[4][3].start_x && touch_x < keys[4][3].end_x &&
							touch_y > keys[4][3].start_y && touch_y < keys[4][3].end_y &&
							pos != 255 && (kUp & KEY_TOUCH)){
						outstr[pos] = ' ';
						touch_x = -1; touch_y = -1;
						pos++;
					}

					//draw text on characters
					if((i < 3 || (i == 3 && j < 8)) || (i == 3 && j == 8) || (i == 4 || j == 0) || (i == 4 || j == 3) || (i == 4 || j == 8)){
						sftd_draw_text(font, keys[i][j].start_x+8, keys[i][j].start_y+4, COLOR_BLACK, 11, keys[i][j].contents);
					}
					//decide if shift is on or not
					if((i < 3 || (i == 3 && j < 8)) && touch_x > keys[4][0].start_x && touch_x < keys[4][0].end_x &&
							touch_y > keys[4][0].start_y && touch_y < keys[4][0].end_y &&
							(kUp & KEY_TOUCH) && keys[i][j].contents[0] == kb_lower_chars[i][j][0])
						keys[i][j].contents = kb_upper_chars[i][j];
					else if((i < 3 || (i == 3 && j < 8)) && (kUp & KEY_TOUCH) && keys[i][j].contents[0] == kb_upper_chars[i][j][0])
						keys[i][j].contents = kb_lower_chars[i][j];
				}
			}
		sf2d_end_frame();

		sf2d_swapbuffers();
	}

	return outstr;
}
