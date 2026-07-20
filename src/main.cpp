#include <Arduino.h>
#include "pins.h"
#include "display.h"
#include "buttons.h"
#include "buzzer.h"
#include "sensors.h"
#include "sd_card.h"

static const uint16_t notes[12] = {
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494
};

static const uint16_t btn_note[6] = {
    notes[0],                // LEFT  → C4  262 Hz
    notes[2],                // UP    → D4  294 Hz
    notes[4],                // DOWN  → E4  330 Hz
    notes[7],                // RIGHT → G4  392 Hz
    notes[9],                // B     → A4  440 Hz
    (uint16_t)(notes[0] * 2) // A     → C5  524 Hz
};

static const uint8_t field2note[6] = {1, 2, 0, 3, 5, 4};

static button_state_t prev = {0, 0, 0, 0, 0, 0};
static bool sd_present = false;

// ── Status line helper ───────────────────────────────────────
// Draws a pre-formatted line at fixed (4, y)
static void draw_line(int y, const char *fmt, ...)
{
    char buf[24];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    tft.fillRect(4, y, 156, 8, TFT_BLACK);
    tft.setCursor(4, y);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print(buf);
}

// Draws "   SD: OK/--" with colored status
static void draw_sd_line(int y, bool present)
{
    tft.fillRect(4, y, 156, 8, TFT_BLACK);
    tft.setCursor(4, y);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("   SD: ");
    tft.setTextColor(present ? TFT_GREEN : TFT_RED, TFT_BLACK);
    tft.print(present ? "OK" : "--");
}

// ── Button drawing ───────────────────────────────────────────
//                  [U]         [A]
//           [L]  ─────  [R]
//                  [D]     [B]

static const struct { int8_t x, y; } btn_pos[6] = {
    {46, 68},   // UP    [U]  18px, center 55
    {46, 92},   // DOWN  [D]  18px, center 55
    {29, 80},   // LEFT  [L]  18px
    {63, 80},   // RIGHT [R]  18px
    {102, 74},  // A     [A]  upper-right of [B]
    {90, 86},   // B     [B]  between RIGHT & DOWN
};

static const char *btn_str[6] = {"[U]", "[D]", "[L]", "[R]", "[A]", "[B]"};

static void draw_btn(int i, bool pressed)
{
    int x = btn_pos[i].x, y = btn_pos[i].y;
    int w = strlen(btn_str[i]) * 6;
    tft.fillRect(x, y, w, 8, TFT_BLACK);
    tft.setCursor(x, y);
    tft.setTextColor(pressed ? TFT_GREEN : TFT_DARKGREY, TFT_BLACK);
    tft.print(btn_str[i]);
}

// ── Setup ────────────────────────────────────────────────────
void setup()
{
    Serial.begin(115200);
    delay(500);

    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);

    sd_present = sd_init();
    if (sd_present) Serial.println("SD card OK");
    else           Serial.println("SD card not found");

    display_init();
    sensors_init();
    buttons_init();
    buzzer_init();

    // Title
    int tw = strlen("XIAO MIAO DEMO") * 6;
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setCursor((160 - tw) / 2, 10);
    tft.print("XIAO MIAO DEMO");

    tft.drawFastHLine(0, 22, 160, TFT_DARKGREY);

    draw_sd_line(28, sd_present);
    draw_line(38, "Light: %u", 0);
    draw_line(48, " Temp: %u", 0);

    tft.drawFastHLine(0, 60, 160, TFT_DARKGREY);

    for (int i = 0; i < 6; i++) draw_btn(i, false);
}

// ── Loop ─────────────────────────────────────────────────────
void loop()
{
    uint16_t light = sensor_read_light();
    uint16_t temp  = sensor_read_temp();

    static uint16_t last_light = 0xFFFF, last_temp = 0xFFFF;

    if (light != last_light) {
        draw_line(38, "Light: %u", light);
        last_light = light;
    }
    if (temp != last_temp) {
        draw_line(48, " Temp: %u", temp);
        last_temp = temp;
    }

    button_state_t cur = buttons_read_debounced();
    bool cur_arr[6]  = {cur.up, cur.down, cur.left, cur.right, cur.a, cur.b};
    bool prev_arr[6] = {prev.up, prev.down, prev.left, prev.right, prev.a, prev.b};

    for (int i = 0; i < 6; i++) {
        if (cur_arr[i] != prev_arr[i]) draw_btn(i, cur_arr[i]);
    }

    bool active[6] = {false};
    for (int i = 0; i < 6; i++) active[field2note[i]] = cur_arr[i];

    int playing = -1;
    for (int i = 0; i < 6; i++) {
        if (active[i]) { playing = i; break; }
    }

    static int prev_playing = -1;
    if (playing != prev_playing) {
        if (playing >= 0) buzzer_tone(btn_note[playing], 0);
        else              buzzer_no_tone();
        prev_playing = playing;
    }

    prev = cur;
    delay(30);
}
