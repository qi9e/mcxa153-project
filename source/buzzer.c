#include "buzzer.h"
#include "fsl_ctimer.h"
#include "fsl_gpio.h"
#include "fsl_clock.h"
#include "fsl_common.h"      /* SDK_DelayAtLeastUs */
#include "fsl_clock.h"
#include "fsl_reset.h"

#define BUZZER_GPIO    GPIO3
#define BUZZER_PIN     8U
#define BUZZER_CTIMER  CTIMER0

#define BUZZER_CTIMER_CLK_HZ  CLOCK_GetCTimerClkFreq(0U)

static void delay_ms(uint32_t ms)
{
    SDK_DelayAtLeastUs(ms * 1000U, SystemCoreClock);
}

static void buzzer_toggle_cb(uint32_t flags)
{
    (void)flags;
    GPIO_PortToggle(BUZZER_GPIO, 1U << BUZZER_PIN);
}

static ctimer_callback_t s_cb[] = { buzzer_toggle_cb };

void buzzer_init(void)
{
    /* 1. 给 CTIMER0 选时钟源 + 开时钟门 + 释放 reset */
	CLOCK_SetClockDiv(kCLOCK_DivCTIMER0, 1U);
    CLOCK_AttachClk(kFRO12M_to_CTIMER0);
    CLOCK_EnableClock(kCLOCK_GateCTIMER0);
    RESET_ReleasePeripheralReset(kCTIMER0_RST_SHIFT_RSTn);

    /* 2. 然后才能安全初始化 */
    ctimer_config_t cfg;
    CTIMER_GetDefaultConfig(&cfg);
    CTIMER_Init(BUZZER_CTIMER, &cfg);
    CTIMER_RegisterCallBack(BUZZER_CTIMER, s_cb, kCTIMER_SingleCallback);

    GPIO_PinWrite(BUZZER_GPIO, BUZZER_PIN, 0);
}

void buzzer_silent(void)
{
    CTIMER_StopTimer(BUZZER_CTIMER);
    BUZZER_CTIMER->TCR |=  CTIMER_TCR_CRST_MASK;
    BUZZER_CTIMER->TCR &= ~CTIMER_TCR_CRST_MASK;
    GPIO_PinWrite(BUZZER_GPIO, BUZZER_PIN, 0);
}

void buzzer_tone_set(uint32_t freq_hz)
{
    if (freq_hz == 0U) { buzzer_silent(); return; }

    /* 关键：清掉上一首留下的 TC */
    CTIMER_StopTimer(BUZZER_CTIMER);
    BUZZER_CTIMER->TCR |=  CTIMER_TCR_CRST_MASK;   /* hold reset */
    BUZZER_CTIMER->TCR &= ~CTIMER_TCR_CRST_MASK;   /* release    */

    uint32_t match = (BUZZER_CTIMER_CLK_HZ / (2U * freq_hz)) - 1U;
    ctimer_match_config_t mc = {
        .matchValue         = match,
        .enableCounterReset = true,
        .enableCounterStop  = false,
        .outControl         = kCTIMER_Output_NoAction,
        .outPinInitState    = false,
        .enableInterrupt    = true,
    };
    CTIMER_SetupMatch(BUZZER_CTIMER, kCTIMER_Match_0, &mc);
    CTIMER_StartTimer(BUZZER_CTIMER);
}

/* play_note 现在就是 tone_set + delay + silent 的组合 */
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
    buzzer_silent();
    delay_ms(15);
}

void play_note_ms_nodelay(uint32_t freq_hz, int dur_ms)
{
    if (freq_hz == 0U || dur_ms <= 0) {
        buzzer_silent();
        if (dur_ms) delay_ms(dur_ms);
        return;
    }
    buzzer_tone_set(freq_hz);
    delay_ms(dur_ms);
    buzzer_silent();
}
