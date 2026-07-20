#include "display.h"
#include "pins.h"

TFT_eSPI tft;

void display_init()
{
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextWrap(true);
}

void display_show_boot_screen()
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setCursor(20, 20);
    tft.print("XiaoMiao");
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(20, 50);
    tft.print("ESP32 Demo");
    tft.setCursor(20, 70);
    tft.print("PlatformIO + TFT_eSPI");
}

void display_show_info(const char *line1, const char *line2, const char *line3)
{
    tft.fillRect(0, 100, 160, 60, TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(0, 100);
    tft.print(line1);
    tft.setCursor(0, 115);
    tft.print(line2);
    tft.setCursor(0, 130);
    tft.print(line3);
}

void display_clear()
{
    tft.fillScreen(TFT_BLACK);
}
