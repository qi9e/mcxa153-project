#include "fsl_device_registers.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"

#include "display.h"
#include "gfx.h"
#include "keypad.h"
#include "boot_anim.h"
#include "buzzer.h"
#include "tetris/src/tetris.h"
#include "tetris/src/tetronimo.h"

#define SCAN_PERIOD_MS 5U

static volatile uint8_t g_tick_flag = 0;

void play_tetris_tune();

void SysTick_Handler(void)
{
    KEYPAD_Scan();
    g_tick_flag = 1;
}

static void draw_screen(const char *input, char last_key)
{
    static char s[2] = { '_', '\0' };
	GFX_Clear(0);

    GFX_DrawString(20, 20, "KEYPAD TEST", 7, 0, 2);

    GFX_DrawString(12, 60, "Last key:", 3, 0, 1);


    GFX_DrawString(12, 95, "Input:", 3, 0, 1);
    GFX_DrawString(12, 112, input, 7, 0, 1);

    GFX_DrawString(5, 148, "B:clear  D:reset", 2, 0, 1);

    if (last_key != 0) {
      	s[0] = last_key;
          GFX_DrawString(0, 0, s, 2, 0, 2);
      }

    GFX_Flush();
}

int main(void)
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    Display_Init();
    buzzer_init();
    GFX_Init();
    KEYPAD_Init();
    SysTick_Config(SystemCoreClock / 200);

    GFX_Clear(0);
    GFX_Flush();

    tetris_init();

    while(1){
        char input = KEYPAD_GetKey();

        GFX_Clear(0);
        tetris_update(0);
        GFX_Flush();

        if (input != KEYPAD_NO_KEY) {
            tetris_input(input);
        }
    }
}
