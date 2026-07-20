#include "buttons.h"
#include "pins.h"

#include <Arduino.h>

static const uint8_t button_pins[6] = {
    BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT, BTN_A, BTN_B
};

const char *button_names[6] = {
    "UP", "DOWN", "LEFT", "RIGHT", "A", "B"
};

void buttons_init()
{
    for (int i = 0; i < 6; i++) {
        pinMode(button_pins[i], INPUT_PULLUP);
    }
}

button_state_t buttons_read()
{
    button_state_t s = {0, 0, 0, 0, 0, 0};
    s.up    = (digitalRead(BTN_UP)    == LOW) ? 1 : 0;
    s.down  = (digitalRead(BTN_DOWN)   == LOW) ? 1 : 0;
    s.left  = (digitalRead(BTN_LEFT)   == LOW) ? 1 : 0;
    s.right = (digitalRead(BTN_RIGHT)  == LOW) ? 1 : 0;
    s.a     = (digitalRead(BTN_A)      == LOW) ? 1 : 0;
    s.b     = (digitalRead(BTN_B)      == LOW) ? 1 : 0;
    return s;
}

button_state_t buttons_read_debounced()
{
    button_state_t s1 = buttons_read();
    delay(5);
    button_state_t s2 = buttons_read();

    button_state_t result;
    result.up    = (s1.up    && s2.up)    ? 1 : 0;
    result.down  = (s1.down  && s2.down)  ? 1 : 0;
    result.left  = (s1.left  && s2.left)  ? 1 : 0;
    result.right = (s1.right && s2.right) ? 1 : 0;
    result.a     = (s1.a     && s2.a)     ? 1 : 0;
    result.b     = (s1.b     && s2.b)     ? 1 : 0;
    return result;
}
