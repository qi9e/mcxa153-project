/*
 * buzzer.h
 *
 *  Created on: 2026年5月27日
 *      Author: dani
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>


#define q1 262
#define q2 294
#define q3 330
#define q4 349
#define q5 392
#define q6 440
#define q7 494
#define c1 523
#define c2 587
#define c3 659
#define c4 698
#define c5 784
#define c6 880
#define c7 988
#define d1 1046
#define d2 1175
#define d3 1318
#define d4 1397
#define d5 1568
#define d6 1760
#define d7 1976

#define REST 0           /* 休止符 */
#define BEAT_MS 500U     /* 一拍 = 500 ms, */

void buzzer_init(void);
void play_note(uint32_t freq_hz, float beats);
void buzzer_silent(void);
void buzzer_tone_set(uint32_t freq_hz);   /* non-blocking: 设了就响, 自己用 buzzer_silent() 停 */



#endif /* BUZZER_H_ */
