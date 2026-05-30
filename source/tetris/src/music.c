#include "notes.h"
#include "buzzer.h"

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
    2,1,1,2,1,1,2,1,1,2,1,1,2,1,1,2,2,2,2,2,2,

    3,1,2,1,1,3,1,2,1,1,2,1,1,2,2,2,2,2,2,

    4,4,4,4,4,4,4,4,
    4,4,4,4,2,2,2,2,6,2
};

const int size = sizeof(melody) / sizeof(melody[0]);

int current_note = 0;

void play_next_note() {
  uint32_t duration_ms = (60000UL * lead_times[current_note]) / (BPM);
  play_note_ms_nodelay(melody[current_note], duration_ms);
  current_note = (current_note + 1) % size;
}
