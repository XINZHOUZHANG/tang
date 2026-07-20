#pragma once

#include <TFT_eSPI.h>

extern TFT_eSPI tft;

void display_init();
void display_show_boot_screen();
void display_show_info(const char *line1, const char *line2, const char *line3);
void display_clear();
