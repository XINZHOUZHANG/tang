#include "buzzer.h"
#include "pins.h"

#include <Arduino.h>

static const uint8_t BUZZER_CHANNEL = 0;
static const uint8_t BUZZER_RESOLUTION = 10;
static const uint32_t BUZZER_FREQ_BASE = 5000;

const uint16_t note_freqs[12] = {
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494
};

void buzzer_init()
{
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ_BASE, BUZZER_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_tone(uint16_t frequency, uint32_t duration_ms)
{
    ledcWriteTone(BUZZER_CHANNEL, frequency);
    if (duration_ms > 0) {
        delay(duration_ms);
        ledcWrite(BUZZER_CHANNEL, 0);
    }
}

void buzzer_no_tone()
{
    ledcWrite(BUZZER_CHANNEL, 0);
}

void buzzer_play_note(const char *note, uint8_t octave, uint32_t duration_ms)
{
    static const char *names = "CCDDEFFGGAAB";
    int semitone = -1;
    for (int i = 0; i < 12; i++) {
        if (names[i] == note[0]) {
            semitone = i;
            if (note[1] == '#') semitone++;
            break;
        }
    }
    if (semitone < 0) return;

    uint16_t freq = note_freqs[semitone];
    while (octave > 4) { freq *= 2; octave--; }
    while (octave < 4) { freq /= 2; octave++; }

    buzzer_tone(freq, duration_ms);
}

void buzzer_beep()
{
    buzzer_tone(1000, 100);
}
