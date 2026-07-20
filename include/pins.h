#pragma once

// TFT (ST7735) — SPI2/VSPI
// RST=19 (shared with SD MISO; reset manually in display.cpp then released)
#define TFT_CS      5
#define TFT_DC      4
#define TFT_SCLK    18
#define TFT_MOSI    23

// SD Card — same SPI bus
#define SD_CS       22

// Buttons (all active LOW with internal pull-up)
#define BTN_UP      2
#define BTN_DOWN    13
#define BTN_LEFT    27
#define BTN_RIGHT   35    // input-only pin
#define BTN_A       34    // input-only pin
#define BTN_B       12    // strapping pin — avoid external pull-high at boot

// Buzzer (PWM via LEDC)
#define BUZZER_PIN  14

// ADC sensors
#define LIGHT_SENSOR 36   // ADC1_CH0, input-only
#define TEMP_SENSOR  39   // ADC1_CH3, input-only

// I2C
#define I2C_SCL     15
#define I2C_SDA     21

// UART0 (native, no USB)
#define UART0_TX    1
#define UART0_RX    3

// Reserved expansion (PH2.0 3P header)
#define GPIO_33     33
#define GPIO_32     32
#define GPIO_26     26
#define GPIO_25     25
