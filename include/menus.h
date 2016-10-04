#ifndef NLSE_MENUS_H
#define NLSE_MENUS_H

//load & save
int get_loaded_status();
void load_garden();
void save_changes();

//main menu
void map_menu();
void player_select();
void villager_select();
void misc_menu();

//map menu
void grass_menu();
void unbury_fossils();
void water_flowers();
void map_tile_editor();

//player select & player menu
void player_menu(int player);
void max_bank(int player);
void change_gender(int player);
void change_tan(int player);
void change_hair_style(int player);
void change_hair_color(int player);
void change_eye_color(int player);
void change_face(int player);

//villager select & villager menu
void villager_menu();
void box_menu();
void reset_villager();
void change_villager();

//misc menu
void unlock_all_pwps();

//list test
void list_test();

#endif
