#pragma once

#include <stdint.h>

void buzzer_init();
void buzzer_tone(uint16_t frequency, uint32_t duration_ms);
void buzzer_no_tone();
void buzzer_play_note(const char *note, uint8_t octave, uint32_t duration_ms);
void buzzer_beep();

extern const uint16_t note_freqs[12];
