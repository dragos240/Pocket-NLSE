#ifndef NLSE_UI_H
#define NLSE_UI_H

extern sf2d_texture* topbar;
extern sf2d_texture* leaf;
extern sf2d_texture* kb_bg;

void ui_init();
void ui_fini();

void ui_frame();
void kb_frame();

#endif
