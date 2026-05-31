/*
 * systemUI.h - 主菜单 UI 系统
 *
 * 菜单项: Settings, GameCard, Tetris, 3D Game, Music
 * 按键 2/8 上下切换, A 确认进入, D/按钮中断 返回主页
 */

#ifndef SYSTEM_UI_H
#define SYSTEM_UI_H

#include <stdint.h>
#include <stdbool.h>

/* ====== 应用 ID ====== */
typedef enum {
    APP_NONE = 0,       /* 停留在菜单 */
    APP_SETTINGS,
    APP_GAMECARD,
    APP_TETRIS,
    APP_3DGAME,
    APP_MUSIC,
    APP_COUNT           /* 菜单项总数 = APP_COUNT - 1 */
} app_id_t;

#define MENU_ITEMS  (APP_COUNT - 1)   /* 5 项 */

/* ====== 系统状态 ====== */
typedef enum {
    SYS_BOOT_ANIM,
    SYS_MENU,
    SYS_APP_RUNNING,
} sys_state_t;

typedef struct {
    sys_state_t state;
    int         selected;       /* 0 .. MENU_ITEMS-1 */
    app_id_t    running_app;    /* 当前运行的 app */
} system_ui_t;

/* ====== API ====== */
void     SystemUI_Init(system_ui_t *ui);
void     SystemUI_OnKey(system_ui_t *ui, char key);
void     SystemUI_GoHome(system_ui_t *ui);     /* D 键或按钮中断调用 */
void     SystemUI_Render(system_ui_t *ui);

#endif /* SYSTEM_UI_H */
