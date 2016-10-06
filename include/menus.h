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

void bank_menu(int player);
	//bank menu
	void select_bank_balance(int player);
	void set_bank_balance(int player, int menuindex);
	void max_bank(int player);
	void wipe_bank(int player);
	void view_bank_ids(int player);
void change_gender(int player);
void change_tan(int player);
void change_hair_style(int player);
void change_hair_color(int player);
void change_eye_color(int player);
void change_face(int player);


//villager select & villager menu
void villager_menu(int villager);

void toggle_boxed(int villager);
void reset_villager(int villager);
void set_villager(int villager);

//misc menu
void unlock_all_pwps();

//debug menus
void list_test();
void view_apt_id();
void display_status_id(int villager);

#endif
