/*
 * buzzer.c  —  CTIMER1 硬件 PWM 驱动 (无 ISR)
 *
 * 引脚: P3_12 -> CTIMER1_MAT2
 * 模式: CTIMER_SetupPwm() 50% 方波, CPU 零开销
 */

#include "buzzer.h"
#include "fsl_ctimer.h"
#include "fsl_clock.h"
#include "fsl_reset.h"
#include "fsl_common.h"

#define BUZZER_CTIMER      CTIMER1
#define BUZZER_OUT_CH      kCTIMER_Match_2   /* P3_12 = CT1_MAT2 */

#define BUZZER_CTIMER_CLK_HZ  CLOCK_GetCTimerClkFreq(1U)

static void delay_ms(uint32_t ms)
{
    SDK_DelayAtLeastUs(ms * 1000U, SystemCoreClock);
}

void buzzer_init(void)
{
    /* 1. 给 CTIMER1 选时钟源 + 开时钟门 + 释放 reset */
    CLOCK_SetClockDiv(kCLOCK_DivCTIMER1, 1U);
    CLOCK_AttachClk(kFRO12M_to_CTIMER1);
    CLOCK_EnableClock(kCLOCK_GateCTIMER1);
    RESET_ReleasePeripheralReset(kCTIMER1_RST_SHIFT_RSTn);

    /* 2. 初始化 CTIMER */
    ctimer_config_t cfg;
    CTIMER_GetDefaultConfig(&cfg);
    CTIMER_Init(BUZZER_CTIMER, &cfg);

    /* 不需要 RegisterCallBack, 不需要 GPIO_PinInit
     * 硬件 PWM 直接通过 MAT2 引脚输出 */
}

void buzzer_silent(void)
{
    CTIMER_StopTimer(BUZZER_CTIMER);
    /* 复位计数器, MAT 输出回到低电平 */
    BUZZER_CTIMER->TCR |=  CTIMER_TCR_CRST_MASK;
    BUZZER_CTIMER->TCR &= ~CTIMER_TCR_CRST_MASK;
}

void buzzer_tone_set(uint32_t freq_hz)
{
    if (freq_hz == 0U) {
        buzzer_silent();
        return;
    }

    /* 先停 + 复位, 避免切频率时出毛刺 */
    buzzer_silent();

    uint32_t clk_hz = BUZZER_CTIMER_CLK_HZ;

    /*
     * CTIMER_SetupPwm 内部:
     *   MR3 = clk / freq - 1          (周期, 所有通道共用)
     *   MR2 = MR3 * (100 - duty) / 100 (MAT2 翻转点)
     *   MCR: MR3 匹配时自动 reset counter
     *   EMR: MR2 匹配时 set, MR3 匹配时 clear -> 50% 方波
     *
     * enableInterrupt = false -> CPU 零开销!
     */
    CTIMER_SetupPwm(BUZZER_CTIMER,
                    kCTIMER_Match_3,    /* pwmPeriodChannel: 用 MR3 做周期 */
                    BUZZER_OUT_CH,      /* matchChannel: MAT2 输出占空比 */
                    50U,                /* 50% */
                    freq_hz,
                    clk_hz,
                    false);             /* 不需要中断 */

    CTIMER_StartTimer(BUZZER_CTIMER);
}

/* === 上层 API, 签名不变, 游戏代码零改动 === */

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
