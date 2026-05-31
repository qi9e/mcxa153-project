/*
 * systemUI.c
 *
 *  Created on: 2026年5月31日
 *      Author: dani
 */


/*
 * systemUI.c - 主菜单 UI 系统
 */

#include "systemUI.h"
#include "gfx.h"
#include <string.h>

/* ====== 调色板索引 ====== */
#define COL_BG       0    /* 黑 */
#define COL_FG       3    /* 白 */
#define COL_SEL_BG   6    /* 蓝: 选中项背景 */
#define COL_SEL_FG   7    /* 黄: 选中项文字 */
#define COL_DIM      2    /* 浅灰 */
#define COL_ACCENT   8    /* 青: 标题 */
#define COL_ICON_1   4    /* 红 */
#define COL_ICON_2   5    /* 绿 */
#define COL_ICON_3  10    /* 橙 */
#define COL_ICON_4   9    /* 品红 */
#define COL_ICON_5  11    /* 紫 */

/* ====== 菜单项定义 ====== */
typedef struct {
    const char *label;
    uint8_t     icon_color;
} menu_entry_t;

static const menu_entry_t menu_entries[MENU_ITEMS] = {
    { "Settings",  COL_ICON_1 },
    { "GameCard",  COL_ICON_2 },
    { "Tetris",    COL_ICON_3 },
    { "3D Game",   COL_ICON_4 },
    { "Music",     COL_ICON_5 },
};

/* ====== 居中绘制辅助 ====== */
static void draw_centered(int y, const char *s, uint8_t fg, uint8_t bg, uint8_t size)
{
    int w = 0;
    for (const char *p = s; *p; p++) w++;
    int px = (GFX_W - w * 6 * size) / 2;
    if (px < 0) px = 0;
    GFX_DrawString(px, y, s, fg, bg, size);
}

/* ====== 画一个小图标方块 ====== */
static void draw_icon(int x, int y, uint8_t color)
{
    GFX_FillRect(x, y, 10, 10, color);
    GFX_DrawRect(x, y, 10, 10, COL_FG);
}

/* ====== API 实现 ====== */

void SystemUI_Init(system_ui_t *ui)
{
    memset(ui, 0, sizeof *ui);
    ui->state       = SYS_BOOT_ANIM;
    ui->selected    = 0;
    ui->running_app = APP_NONE;
}

void SystemUI_OnKey(system_ui_t *ui, char key)
{
    /* D 键: 任何状态都返回主页 */
    if (key == 'D') {
        SystemUI_GoHome(ui);
        return;
    }

    if (ui->state == SYS_MENU) {
        switch (key) {
        case '2':   /* 上 */
            if (ui->selected > 0)
                ui->selected--;
            else
                ui->selected = MENU_ITEMS - 1;  /* 循环 */
            break;

        case '8':   /* 下 */
            if (ui->selected < MENU_ITEMS - 1)
                ui->selected++;
            else
                ui->selected = 0;               /* 循环 */
            break;

        case 'A':   /* 确认 */
            ui->running_app = (app_id_t)(ui->selected + 1);
            ui->state = SYS_APP_RUNNING;
            break;
        }
    }
    /* SYS_APP_RUNNING 时按键由具体 app 处理, 这里不截获 */
}

void SystemUI_GoHome(system_ui_t *ui)
{
    ui->state       = SYS_MENU;
    ui->running_app = APP_NONE;
}

void SystemUI_Render(system_ui_t *ui)
{
    if (ui->state != SYS_MENU) return;

    GFX_Clear(COL_BG);

    /* ---- 标题 ---- */
    draw_centered(4, "GameBoy", COL_ACCENT, COL_BG, 2);

    /* ---- 分隔线 ---- */
    GFX_DrawHLine(4, 24, GFX_W - 8, COL_DIM);

    /* ---- 菜单项 ----
     * 每项高 22px, 从 y=30 开始
     * 5 项 * 22 = 110px, 加上标题 30px, 底部提示 20px → 刚好 160px */
    const int item_h  = 22;
    const int start_y = 30;

    for (int i = 0; i < MENU_ITEMS; i++) {
        int y = start_y + i * item_h;
        bool is_sel = (i == ui->selected);

        if (is_sel) {
            /* 选中: 蓝色背景条 */
            GFX_FillRect(2, y, GFX_W - 4, item_h - 2, COL_SEL_BG);
        }

        /* 图标 */
        draw_icon(6, y + 5, menu_entries[i].icon_color);

        /* 文字 */
        uint8_t fg = is_sel ? COL_SEL_FG : COL_FG;
        uint8_t bg = is_sel ? COL_SEL_BG : COL_BG;
        GFX_DrawString(20, y + 6, menu_entries[i].label, fg, bg, 1);

        /* 选中项右边画一个小箭头 ">" */
        if (is_sel) {
            int arrow_x = GFX_W - 14;
            GFX_DrawChar(arrow_x, y + 6, '>', COL_SEL_FG, COL_SEL_BG, 1);
        }
    }

    /* ---- 底部提示 ---- */
    draw_centered(146, "2/8:Nav A:Go D:Home", COL_DIM, COL_BG, 1);

    GFX_Flush();
}
