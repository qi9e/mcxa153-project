#include "buzzer.h"
#include "fsl_ctimer.h"
#include "fsl_clock.h"
#include "fsl_reset.h"
#include "fsl_common.h"

#define BUZZER_CTIMER      CTIMER1
#define BUZZER_OUT_CH      kCTIMER_Match_2  

#define BUZZER_CTIMER_CLK_HZ  CLOCK_GetCTimerClkFreq(1U)

static void delay_ms(uint32_t ms)
{
    SDK_DelayAtLeastUs(ms * 1000U, SystemCoreClock);
}

void buzzer_init(void)
{
    CLOCK_SetClockDiv(kCLOCK_DivCTIMER1, 1U);
    CLOCK_AttachClk(kFRO12M_to_CTIMER1);
    CLOCK_EnableClock(kCLOCK_GateCTIMER1);
    RESET_ReleasePeripheralReset(kCTIMER1_RST_SHIFT_RSTn);

    ctimer_config_t cfg;
    CTIMER_GetDefaultConfig(&cfg);
    CTIMER_Init(BUZZER_CTIMER, &cfg);
}

void buzzer_silent(void)
{
    CTIMER_StopTimer(BUZZER_CTIMER);
    /* counter重复weizhi*/
    BUZZER_CTIMER->TCR |=  CTIMER_TCR_CRST_MASK;
    BUZZER_CTIMER->TCR &= ~CTIMER_TCR_CRST_MASK;
}

void buzzer_tone_set(uint32_t freq_hz)
{
    if (freq_hz == 0U) {
        buzzer_silent();
        return;
    }

    buzzer_silent();

    uint32_t clk_hz = BUZZER_CTIMER_CLK_HZ;

    CTIMER_SetupPwm(BUZZER_CTIMER,
                    kCTIMER_Match_3,
                    BUZZER_OUT_CH,
                    50U,
                    freq_hz,
                    clk_hz,
                    false);

    CTIMER_StartTimer(BUZZER_CTIMER);
}

void play_note(uint32_t freq_hz, float beats)
{
    uint32_t dur_ms = (uint32_t)(beats * BEAT_MS);
    if (freq_hz == 0U || beats <= 0.0f) {
        buzzer_silent();
        if (dur_ms) delay_ms(dur_ms);
        return;
    }
    buzzer_tone_set(freq_hz);
    delay_ms(dur_ms);
}

void begin_note(uint32_t freq_hz)
{
    buzzer_tone_set(freq_hz);
}

void end_note(void)
{
    buzzer_silent();
}
