#include "sd_card.h"
#include "pins.h"

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

static SPIClass sdSPI(VSPI);

bool sd_init()
{
    sdSPI.begin(TFT_SCLK, 19, TFT_MOSI, -1);

    digitalWrite(SD_CS, HIGH);
    for (int i = 0; i < 10; i++) sdSPI.transfer(0xFF);
    delay(10);

    return SD.begin(SD_CS, sdSPI, 4000000);
}

bool sd_check()
{
    sdSPI.begin(TFT_SCLK, 19, TFT_MOSI, -1);
    bool ok = SD.begin(SD_CS, sdSPI, 20000000);

    // Restore HSPI for TFT (SCK/MOSI only; TFT doesn't use MISO)
    SPI.begin(TFT_SCLK, -1, TFT_MOSI, -1);
    // Restore GPIO19 as TFT RST (inactive HIGH)
    pinMode(19, OUTPUT);
    digitalWrite(19, HIGH);

    return ok;
}

bool sd_list_files()
{
    File root = SD.open("/");
    if (!root) {
        Serial.println("SD: failed to open root");
        return false;
    }
    if (!root.isDirectory()) {
        Serial.println("SD: root is not a directory");
        root.close();
        return false;
    }

    Serial.println("SD: root directory:");
    int count = 0;
    File entry = root.openNextFile();
    while (entry) {
        count++;
        Serial.print("  ");
        Serial.print(entry.isDirectory() ? "[DIR] " : "      ");
        Serial.println(entry.name());
        entry.close();
        entry = root.openNextFile();
    }
    Serial.printf("SD: %d entries found\n", count);
    root.close();
    return true;
}
