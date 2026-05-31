/*
 * buzzer.h  —  CTIMER0 硬件 PWM 蜂鸣器驱动
 *
 * 引脚: P3_12 (CTIMER0_MAT0 输出)
 * 模式: 硬件 PWM, 50% 占空比方波, 无中断
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

/* ====== 音符频率表 (Hz) ====== */
/* 低音 (q = 低八度) */
#define q1 262
#define q2 294
#define q3 330
#define q4 349
#define q5 392
#define q6 440
#define q7 494

/* 中音 (c = 中八度) */
#define c1 523
#define c2 587
#define c3 659
#define c4 698
#define c5 784
#define c6 880
#define c7 988

/* 高音 (d = 高八度) */
#define d1 1046
#define d2 1175
#define d3 1318
#define d4 1397
#define d5 1568
#define d6 1760
#define d7 1976

#define REST    0U       /* 休止符 */
#define BEAT_MS 500U     /* 一拍 = 500 ms */

/* ====== API ====== */
void buzzer_init(void);
void buzzer_silent(void);

/* 设置频率, non-blocking: 设了就响, 调 buzzer_silent() 停 */
void buzzer_tone_set(uint32_t freq_hz);

/* 阻塞式: 响 beats 拍后自动停 */
void play_note(uint32_t freq_hz, float beats);

/* 非阻塞式: 游戏主循环用 */
void begin_note(uint32_t freq_hz);
void end_note(void);

#endif /* BUZZER_H */
