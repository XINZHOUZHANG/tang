#pragma once

#include <stdint.h>

typedef struct {
    uint8_t up    : 1;
    uint8_t down  : 1;
    uint8_t left  : 1;
    uint8_t right : 1;
    uint8_t a     : 1;
    uint8_t b     : 1;
} button_state_t;

extern const char *button_names[6];

void buttons_init();
button_state_t buttons_read();
button_state_t buttons_read_debounced();
