#include "boot_anim.h"
#include "gfx.h"
#include "buzzer.h"
#include "fsl_common.h"
#include <stdint.h>

#define COL_BG      0
#define COL_WHITE   3
#define COL_RED     4
#define COL_GREEN   5
#define COL_BLUE    6
#define COL_YELLOW  7
#define COL_DIM     2

static void delay_anim(uint32_t t)
{
    volatile uint32_t i;
    while (t--) {
        for (i = 0; i < 2500; i++) {
            __NOP();
        }
    }
}

/* 圆轨迹查表，不用 math.h */
static const int8_t cx_tbl[32] = {
     30, 29, 28, 25, 21, 17, 11,  6,
      0, -6,-11,-17,-21,-25,-28,-29,
    -30,-29,-28,-25,-21,-17,-11, -6,
      0,  6, 11, 17, 21, 25, 28, 29
};

static const int8_t cy_tbl[32] = {
      0,  6, 11, 17, 21, 25, 28, 29,
     30, 29, 28, 25, 21, 17, 11,  6,
      0, -6,-11,-17,-21,-25,-28,-29,
    -30,-29,-28,-25,-21,-17,-11, -6
};

void BootAnimation_Run(void)
{
    const int center_x = GFX_W / 2;
    const int center_y = 68;

    const uint8_t colors[4] = {
        COL_RED, COL_GREEN, COL_BLUE, COL_YELLOW
    };

    /* 每个球不同中心点，所以轨迹不同 */
    const int orbit_cx[4] = {
        center_x - 35,
        center_x + 35,
        center_x - 28,
        center_x + 28
    };

    const int orbit_cy[4] = {
        center_y - 25,
        center_y - 20,
        center_y + 25,
        center_y + 25
    };

    const int final_x[4] = {
        center_x - 10,
        center_x + 10,
        center_x - 10,
        center_x + 10
    };

    const int final_y[4] = {
        center_y - 10,
        center_y - 10,
        center_y + 10,
        center_y + 10
    };

    /* 1) 四个球各自旋转，同时慢慢靠近中心
     *    音效: C 大调上升, 每 20 帧换一个音 (C4 -> E4 -> G4 -> C5) */
    for (int frame = 0; frame <= 80; frame++) {
        if (frame == 0)       buzzer_tone_set(q1);   /* C4  262 */
        else if (frame == 20) buzzer_tone_set(q3);   /* E4  330 */
        else if (frame == 40) buzzer_tone_set(q5);   /* G4  392 */
        else if (frame == 60) buzzer_tone_set(c1);   /* C5  523 */

        GFX_Clear(COL_BG);

        for (int b = 0; b < 4; b++) {
            int idx = (frame + b * 8) % 32;

            int ox = orbit_cx[b] + cx_tbl[idx];
            int oy = orbit_cy[b] + cy_tbl[idx];

            int x = ox + (final_x[b] - ox) * frame / 80;
            int y = oy + (final_y[b] - oy) * frame / 80;

            /* 轨迹小点 */
            for (int t = 0; t < 32; t += 4) {
                int tx = orbit_cx[b] + cx_tbl[(t + b * 3) % 32];
                int ty = orbit_cy[b] + cy_tbl[(t + b * 3) % 32];

                tx = tx + (final_x[b] - tx) * frame / 80;
                ty = ty + (final_y[b] - ty) * frame / 80;

                GFX_FillCircle(tx, ty, 1, COL_DIM);
            }

            GFX_FillCircle(x, y, 5, colors[b]);
        }

        GFX_DrawString(34, 132, "Loading", COL_DIM, COL_BG, 1);

        GFX_Flush();
        delay_anim(5);
    }

    /* 阶段 1 / 2 之间留个小停顿，上升感更分明 */
    buzzer_silent();
    delay_anim(15);

    /* 2) 中心形成四色图标，下面出现 GameBoy
     *    音效: 上升琶音 (C5 -> E5 -> G5 -> C6) */
    for (int pulse = 0; pulse < 20; pulse++) {
    	if (pulse % 2 == 0)
    	{
    	    switch (pulse / 2)
    	    {
    	        case 1: buzzer_tone_set(c3); break;
    	        case 2: buzzer_tone_set(c4); break;
    	        case 3: buzzer_tone_set(c5); break;
    	        case 4: buzzer_tone_set(c6); break;
    	        case 5: buzzer_tone_set(d1); break;
    	    }
    	}

        GFX_Clear(COL_BG);

        int r = 6 + (pulse % 3);

        GFX_FillCircle(center_x - 10, center_y - 10, r, COL_RED);
        GFX_FillCircle(center_x + 10, center_y - 10, r, COL_GREEN);
        GFX_FillCircle(center_x - 10, center_y + 10, r, COL_BLUE);
        GFX_FillCircle(center_x + 10, center_y + 10, r, COL_YELLOW);

        GFX_DrawString(40, 118, "GameBoy", COL_WHITE, COL_BG, 1);

        GFX_Flush();
        delay_anim(10);
    }

    /* 3) Welcome 结束画面: 高音 C6 余韵, 然后静音 */
    GFX_Clear(COL_BG);

    GFX_FillCircle(center_x - 10, center_y - 10, 6, COL_RED);
    GFX_FillCircle(center_x + 10, center_y - 10, 6, COL_GREEN);
    GFX_FillCircle(center_x - 10, center_y + 10, 6, COL_BLUE);
    GFX_FillCircle(center_x + 10, center_y + 10, 6, COL_YELLOW);

    GFX_DrawString(40, 112, "GameBoy", COL_WHITE, COL_BG, 1);
    GFX_DrawString(40, 132, "Welcome", COL_WHITE, COL_BG, 1);

    GFX_Flush();

    /* C6 拖个尾, 大约 200 ms 后归静 */
    delay_anim(60);
    buzzer_silent();
}
