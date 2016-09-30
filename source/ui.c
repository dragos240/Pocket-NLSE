#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>

sf2d_texture* topbar;
sf2d_texture* leaf;
sf2d_texture* kb_bg;

void ui_init(){
	sf2d_set_clear_color(RGBA8(0x00, 0x2e, 0x00, 0xFF));

	topbar = sfil_load_PNG_file("romfs:/topbar.png", SF2D_PLACE_RAM);
	leaf = sfil_load_PNG_file("romfs:/leaf.png", SF2D_PLACE_RAM);
	kb_bg = sfil_load_PNG_file("romfs:/kb_bg.png", SF2D_PLACE_RAM);
}

void ui_fini(){
	sf2d_free_texture(kb_bg);
	sf2d_free_texture(leaf);
	sf2d_free_texture(topbar);
}

void ui_frame(){
	sf2d_draw_texture(topbar, 0, 0);
	sf2d_draw_texture(leaf, 287, 22);
}

void kb_frame(){
	sf2d_draw_texture(kb_bg, 0, 0);
}
