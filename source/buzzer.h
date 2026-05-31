#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

#define REST    0U
#define BEAT_MS 500U

void buzzer_init(void);
void buzzer_silent(void);
void buzzer_tone_set(uint32_t freq_hz);
void play_note(uint32_t freq_hz, float beats);
void begin_note(uint32_t freq_hz);
void end_note(void);

#endif /* BUZZER_H */
