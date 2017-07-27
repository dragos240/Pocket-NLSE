#pragma once

#include <3ds.h>

#define TOP_SCREEN_WIDTH 400
#define TOP_SCREEN_HEIGHT 240

#define BOTTOM_SCREEN_WIDTH 320
#define BOTTOM_SCREEN_HEIGHT 240

#define MAX_TEXTURES 1024

#define TEXTURE_TOP_SCREEN_BG 1
#define TEXTURE_BOTTOM_SCREEN_BG 2
#define TEXTURE_TOP_SCREEN_TOP_BAR 3
#define TEXTURE_TOP_SCREEN_TOP_BAR_SHADOW 4
#define TEXTURE_BOTTOM_SCREEN_TOP_BAR 5
#define TEXTURE_BOTTOM_SCREEN_TOP_BAR_SHADOW 6
#define TEXTURE_BOTTOM_SCREEN_BOTTOM_BAR 7
#define TEXTURE_BOTTOM_SCREEN_BOTTOM_BAR_SHADOW 8
#define TEXTURE_LOGO 9
#define TEXTURE_SCROLL_BAR 10
#define TEXTURE_SELECTION_OVERLAY 11
#define TEXTURE_BUTTON_SMALL 12
#define TEXTURE_BUTTON_LARGE 13
#define TEXTURE_POCKETS 14
#define TEXTURE_ITEM_OCCUPIED 15
#define TEXTURE_MAP_BACK 16

#define TEXTURE_AUTO_START 32

void screen_init();
void screen_exit();
void screen_load_texture(u32 id, void* data, u32 size, u32 width, u32 height, GPU_TEXCOLOR format, bool linearFilter);
u32 screen_load_texture_auto(void* data, u32 size, u32 width, u32 height, GPU_TEXCOLOR format, bool linearFilter);
void screen_load_texture_file(u32 id, const char* path, bool linearFilter);
u32 screen_load_texture_file_auto(const char* path, bool linearFilter);
void screen_load_texture_tiled(u32 id, void* tiledData, u32 size, u32 width, u32 height, GPU_TEXCOLOR format, bool linearFilter);
u32 screen_load_texture_tiled_auto(void* tiledData, u32 size, u32 width, u32 height, GPU_TEXCOLOR format, bool linearFilter);
void screen_unload_texture(u32 id);
void screen_get_texture_size(u32* width, u32* height, u32 id);
void screen_begin_frame();
void screen_end_frame();
void screen_select(gfxScreen_t screen);
void screen_set_scissor(bool enabled, u32 x, u32 y, u32 width, u32 height);
void screen_draw_texture(u32 id, float x, float y, float width, float height);
void screen_draw_texture_crop(u32 id, float x, float y, float width, float height);
void screen_get_string_size(float* width, float* height, const char* text, float scaleX, float scaleY);
void screen_get_string_size_wrap(float* width, float* height, const char* text, float scaleX, float scaleY, float wrapX);
void screen_draw_string(const char* text, float x, float y, float scaleX, float scaleY, u32 colorId, bool centerLines);
void screen_draw_string_wrap(const char* text, float x, float y, float scaleX, float scaleY, u32 colorId, bool centerLines, float wrapX);
