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
#include <stdio.h>

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
    //KEYPAD_Init();

    GFX_Clear(0);
    GFX_Flush();

    //BootAnimation_Run();

    tetris_init();

    while(1){
    	/*
    	KEYPAD_Scan();
    	char input = KEYPAD_GetKey();


    	GFX_Clear(0);
    	tetris_update(0);
    	GFX_Flush();
    	printf("%c",input);

    	if ( input != KEYPAD_NO_KEY)
    	{
    		tetris_input(input);

    	}
		*/
    	play_tetris_tune();

    }
}

#define _C0    16
#define _CS0   17
#define _D0    18
#define _DS0   19
#define _E0    21
#define _F0    22
#define _FS0   23
#define _G0    25
#define _GS0   26
#define _A0    28
#define _AS0   29
#define _B0    31

#define _C1    33
#define _CS1   35
#define _D1    37
#define _DS1   39
#define _E1    41
#define _F1    44
#define _FS1   46
#define _G1    49
#define _GS1   52
#define _A1    55
#define _AS1   58
#define _B1    62

#define _C2    65
#define _CS2   69
#define _D2    73
#define _DS2   78
#define _E2    82
#define _F2    87
#define _FS2   93
#define _G2    98
#define _GS2   104
#define _A2    110
#define _AS2   117
#define _B2    123

#define _C3    131
#define _CS3   139
#define _D3    147
#define _DS3   156
#define _E3    165
#define _F3    175
#define _FS3   185
#define _G3    196
#define _GS3   208
#define _A3    220
#define _AS3   233
#define _B3    247

#define _C4    262
#define _CS4   277
#define _D4    294
#define _DS4   311
#define _E4    330
#define _F4    349
#define _FS4   370
#define _G4    392
#define _GS4   415
#define _A4    440
#define _AS4   466
#define _B4    494

#define _C5    523
#define _CS5   554
#define _D5    587
#define _DS5   622
#define _E5    659
#define _F5    698
#define _FS5   740
#define _G5    784
#define _GS5   831
#define _A5    880
#define _AS5   932
#define _B5    988

#define _C6    1047
#define _CS6   1109
#define _D6    1175
#define _DS6   1245
#define _E6    1319
#define _F6    1397
#define _FS6   1480
#define _G6    1568
#define _GS6   1661
#define _A6    1760
#define _AS6   1865
#define _B6    1976

#define _C7    2093
#define _CS7   2217
#define _D7    2349
#define _DS7   2489
#define _E7    2637
#define _F7    2794
#define _FS7   2960
#define _G7    3136
#define _GS7   3322
#define _A7    3520
#define _AS7   3729
#define _B7    3951

#define _C8    4186
#define _CS8   4435
#define _D8    4699
#define _DS8   4978
#define _E8    5274
#define _F8    5588
#define _FS8   5920
#define _G8    6272
#define _GS8   6645
#define _A8    7040
#define _AS8   7459
#define _B8    7902
#define _R 0

#define BPM 120


uint16_t melody[] = {
    _E5, _B4, _C5, _D5, _C5, _B4, _A4, _A4, _C5, _E5,
    _D5, _C5, _B4, _B4, _C5, _D5, _E5, _C5, _A4, _A4, _R,

    _D5, _F5, _A5, _G5, _F5, _E5, _C5, _E5, _D5, _C5,
    _B4, _B4, _C5, _D5, _E5, _C5, _A4, _A4, _R,

    _E4, _C4, _D4, _B3, _C4, _A3, _GS3, _B3,
    _E4, _C4, _D4, _B3, _C4, _E4, _A4, _A4, _GS4, _R
};

uint8_t lead_times[] = {
    // value = beats * 2
    2,1,1,2,1,1,2,1,1,2,1,1,2,1,1,2,2,2,2,2,2,

    3,1,2,1,1,3,1,2,1,1,2,1,1,2,2,2,2,2,2,

    4,4,4,4,4,4,4,4,
    4,4,4,4,2,2,2,2,6,2
};

void play_tetris_tune() {
  const int size = sizeof(melody) / sizeof(melody[0]);
  for (int i = 0; i < size; i++) {
    uint32_t duration_ms = (60000UL * lead_times[i]) / (BPM);
    play_note_ms(melody[i], duration_ms);
  }
}


