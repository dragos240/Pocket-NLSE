#include <stdio.h>
#include <string.h>
#include <time.h>

#include <3ds.h>
#include <malloc.h>
#include <stdarg.h>

#include "ui/ui.h"
#include "core/screen.h"
#include "core/common.h"
#include "core/util.h"
#include "ui/prompt.h"

#define MAX_UI_VIEWS 16

static ui_view* ui_stack[MAX_UI_VIEWS];
static int ui_stack_top = -1;

static Handle ui_stack_mutex = 0;

void ui_init() {
    if(ui_stack_mutex == 0) {
        svcCreateMutex(&ui_stack_mutex, false);
    }
}

void ui_exit() {
    if(ui_stack_mutex != 0) {
        svcCloseHandle(ui_stack_mutex);
        ui_stack_mutex = 0;
    }
}

ui_view* ui_create() {
    ui_view* view = (ui_view*) calloc(1, sizeof(ui_view));
    if(view == NULL) {
        util_panic("Failed to allocate UI view.");
        return NULL;
    }

    Result res = 0;
    if(R_FAILED(res = svcCreateEvent(&view->active, RESET_STICKY))) {
        util_panic("Failed to create view active event: 0x%08lX", res);

        free(view);
        return NULL;
    }

    return view;
}

void ui_destroy(ui_view* view) {
    if(view != NULL) {
        svcCloseHandle(view->active);
        free(view);
    }
}

ui_view* ui_top() {
    svcWaitSynchronization(ui_stack_mutex, U64_MAX);

    ui_view* ui = NULL;
    if(ui_stack_top >= 0) {
        ui = ui_stack[ui_stack_top];
    }

    svcReleaseMutex(ui_stack_mutex);

    return ui;
}

bool ui_push(ui_view* view) {
    if(view == NULL) {
        return false;
    }

    svcWaitSynchronization(ui_stack_mutex, U64_MAX);

    bool space = ui_stack_top < MAX_UI_VIEWS - 1;
    if(space) {
        ui_stack[++ui_stack_top] = view;

        svcClearEvent(view->active);
    }

    svcReleaseMutex(ui_stack_mutex);

    return space;
}

void ui_pop() {
    svcWaitSynchronization(ui_stack_mutex, U64_MAX);

    if(ui_stack_top >= 0) {
        svcSignalEvent(ui_stack[ui_stack_top]->active);

        ui_stack[ui_stack_top--] = NULL;
    }

    svcReleaseMutex(ui_stack_mutex);
}

static void ui_draw_top(ui_view* ui) {
    u32 topScreenBgWidth = 0;
    u32 topScreenBgHeight = 0;
    screen_get_texture_size(&topScreenBgWidth, &topScreenBgHeight, TEXTURE_TOP_SCREEN_BG);

    u32 topScreenTopBarWidth = 0;
    u32 topScreenTopBarHeight = 0;
    screen_get_texture_size(&topScreenTopBarWidth, &topScreenTopBarHeight, TEXTURE_TOP_SCREEN_TOP_BAR);

    u32 topScreenTopBarShadowWidth = 0;
    u32 topScreenTopBarShadowHeight = 0;
    screen_get_texture_size(&topScreenTopBarShadowWidth, &topScreenTopBarShadowHeight, TEXTURE_TOP_SCREEN_TOP_BAR_SHADOW);

    screen_select(GFX_TOP);
    screen_draw_texture(TEXTURE_TOP_SCREEN_BG, (TOP_SCREEN_WIDTH - topScreenBgWidth) / 2, (TOP_SCREEN_HEIGHT - topScreenBgHeight) / 2, topScreenBgWidth, topScreenBgHeight);

    if(ui->drawTop != NULL) {
        ui->drawTop(ui, ui->data, 0, topScreenTopBarHeight, TOP_SCREEN_WIDTH, TOP_SCREEN_HEIGHT);
    }

    float topScreenTopBarX = (TOP_SCREEN_WIDTH - topScreenTopBarWidth) / 2;
    float topScreenTopBarY = 0;
    screen_draw_texture(TEXTURE_TOP_SCREEN_TOP_BAR, topScreenTopBarX, topScreenTopBarY, topScreenTopBarWidth, topScreenTopBarHeight);
    screen_draw_texture(TEXTURE_TOP_SCREEN_TOP_BAR_SHADOW, topScreenTopBarX, topScreenTopBarY + topScreenTopBarHeight, topScreenTopBarShadowWidth, topScreenTopBarShadowHeight);

    char verText[64];
    snprintf(verText, 64, "Ver. %d.%d.%d", 1, 2, 0);

    float verWidth;
    float verHeight;
    screen_get_string_size(&verWidth, &verHeight, verText, 0.5f, 0.5f);
    screen_draw_string(verText, topScreenTopBarX + 2, topScreenTopBarY + (topScreenTopBarHeight - verHeight) / 2, 0.5f, 0.5f, COLOR_BLACK, true);

    time_t t = time(NULL);
    char* timeText = ctime(&t);
    timeText[strlen(timeText) - 1] = '\0';

    float timeTextWidth;
    float timeTextHeight;
    screen_get_string_size(&timeTextWidth, &timeTextHeight, timeText, 0.5f, 0.5f);
    screen_draw_string(timeText, topScreenTopBarX + (topScreenTopBarWidth - timeTextWidth) / 2, topScreenTopBarY + (topScreenTopBarHeight - timeTextHeight) / 2, 0.5f, 0.5f, COLOR_BLACK, true);
}

static void ui_draw_bottom(ui_view* ui) {
    u32 bottomScreenBgWidth = 0;
    u32 bottomScreenBgHeight = 0;
    screen_get_texture_size(&bottomScreenBgWidth, &bottomScreenBgHeight, TEXTURE_BOTTOM_SCREEN_BG);

    u32 bottomScreenTopBarWidth = 0;
    u32 bottomScreenTopBarHeight = 0;
    screen_get_texture_size(&bottomScreenTopBarWidth, &bottomScreenTopBarHeight, TEXTURE_BOTTOM_SCREEN_TOP_BAR);

    u32 bottomScreenTopBarShadowWidth = 0;
    u32 bottomScreenTopBarShadowHeight = 0;
    screen_get_texture_size(&bottomScreenTopBarShadowWidth, &bottomScreenTopBarShadowHeight, TEXTURE_BOTTOM_SCREEN_TOP_BAR_SHADOW);

    u32 bottomScreenBottomBarWidth = 0;
    u32 bottomScreenBottomBarHeight = 0;
    screen_get_texture_size(&bottomScreenBottomBarWidth, &bottomScreenBottomBarHeight, TEXTURE_BOTTOM_SCREEN_BOTTOM_BAR);

    u32 bottomScreenBottomBarShadowWidth = 0;
    u32 bottomScreenBottomBarShadowHeight = 0;
    screen_get_texture_size(&bottomScreenBottomBarShadowWidth, &bottomScreenBottomBarShadowHeight, TEXTURE_BOTTOM_SCREEN_BOTTOM_BAR_SHADOW);

    screen_select(GFX_BOTTOM);
    screen_draw_texture(TEXTURE_BOTTOM_SCREEN_BG, (BOTTOM_SCREEN_WIDTH - bottomScreenBgWidth) / 2, (BOTTOM_SCREEN_HEIGHT - bottomScreenBgHeight) / 2, bottomScreenBgWidth, bottomScreenBgHeight);

    if(ui->drawBottom != NULL) {
        ui->drawBottom(ui, ui->data, 0, bottomScreenTopBarHeight, BOTTOM_SCREEN_WIDTH, BOTTOM_SCREEN_HEIGHT - bottomScreenBottomBarHeight);
    }

    float bottomScreenTopBarX = (BOTTOM_SCREEN_WIDTH - bottomScreenTopBarWidth) / 2;
    float bottomScreenTopBarY = 0;
    screen_draw_texture(TEXTURE_BOTTOM_SCREEN_TOP_BAR, bottomScreenTopBarX, bottomScreenTopBarY, bottomScreenTopBarWidth, bottomScreenTopBarHeight);
    screen_draw_texture(TEXTURE_BOTTOM_SCREEN_TOP_BAR_SHADOW, bottomScreenTopBarX, bottomScreenTopBarY + bottomScreenTopBarHeight, bottomScreenTopBarShadowWidth, bottomScreenTopBarShadowHeight);

    float bottomScreenBottomBarX = (BOTTOM_SCREEN_WIDTH - bottomScreenBottomBarWidth) / 2;
    float bottomScreenBottomBarY = BOTTOM_SCREEN_HEIGHT - bottomScreenBottomBarHeight;
    screen_draw_texture(TEXTURE_BOTTOM_SCREEN_BOTTOM_BAR, bottomScreenBottomBarX, bottomScreenBottomBarY, bottomScreenBottomBarWidth, bottomScreenBottomBarHeight);
    screen_draw_texture(TEXTURE_BOTTOM_SCREEN_BOTTOM_BAR_SHADOW, bottomScreenBottomBarX, bottomScreenBottomBarY - bottomScreenBottomBarShadowHeight, bottomScreenBottomBarShadowWidth, bottomScreenBottomBarShadowHeight);

    if(ui->name != NULL) {
        float nameWidth;
        float nameHeight;
        screen_get_string_size(&nameWidth, &nameHeight, ui->name, 0.5f, 0.5f);
        screen_draw_string(ui->name, (BOTTOM_SCREEN_WIDTH - nameWidth) / 2, (bottomScreenTopBarHeight - nameHeight) / 2, 0.5f, 0.5f, COLOR_BLACK, true);
    }

    if(ui->info != NULL) {
        float infoWidth;
        float infoHeight;
        screen_get_string_size(&infoWidth, &infoHeight, ui->info, 0.5f, 0.5f);
        screen_draw_string(ui->info, (BOTTOM_SCREEN_WIDTH - infoWidth) / 2, BOTTOM_SCREEN_HEIGHT - (bottomScreenBottomBarHeight + infoHeight) / 2, 0.5f, 0.5f, COLOR_BLACK, true);
    }
}

bool ui_update() {
    ui_view* ui = NULL;

    hidScanInput();

    ui = ui_top();
    if(ui != NULL && ui->update != NULL) {
        u32 bottomScreenTopBarHeight = 0;
        screen_get_texture_size(NULL, &bottomScreenTopBarHeight, TEXTURE_BOTTOM_SCREEN_TOP_BAR);

        u32 bottomScreenBottomBarHeight = 0;
        screen_get_texture_size(NULL, &bottomScreenBottomBarHeight, TEXTURE_BOTTOM_SCREEN_BOTTOM_BAR);

        ui->update(ui, ui->data, 0, bottomScreenTopBarHeight, BOTTOM_SCREEN_WIDTH, BOTTOM_SCREEN_HEIGHT - bottomScreenBottomBarHeight);
    }

    ui = ui_top();
    if(ui != NULL) {
        screen_begin_frame();
        ui_draw_top(ui);
        ui_draw_bottom(ui);
        screen_end_frame();
    }

    return ui != NULL;
}

void gfx_prompt(char* message){
	prompt_display("Attention", message, COLOR_BLACK, true, NULL, NULL, NULL);
}

void gfx_waitmessage(char* msg, ...){
	char buffer[256];
	va_list args;
	va_start(args, msg);
	vsnprintf(buffer, 256, msg, args);
	prompt_display("Success", msg, COLOR_BLACK, false, NULL, NULL, NULL);
	va_end(args);
}
