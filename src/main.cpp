#include <Arduino.h>
#include <SD.h>
#include <esp_ota_ops.h>

// 引入底层封装好的硬件模块
#include "pins.h"
#include "display.h"
#include "buttons.h"
#include "buzzer.h"

// 注意：由于底层换成了 TFT_eSPI，要在屏幕上渲染中文字体，
// 你后续需要在 platformio.ini 的 lib_deps 中引入 U8g2_for_TFT_eSPI 库。
// #include <U8g2_for_TFT_eSPI.h>
// U8G2_FOR_TFT_eSPI u8g2Fonts;

// ==========================================
// 核心功能：返回 Launcher 启动器
// ==========================================
void returnToLauncher() {
    Serial.println("Preparing to exit to Launcher...");

    // 调用现成的屏幕模块清屏并显示退出提示
    display_clear();
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(10, 60);
    tft.print("Exiting to Menu...");

    // 寻找出厂分区（Factory Partition），这是 Launcher 系统的驻留位置
    const esp_partition_t *factory_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);

    if (factory_partition != NULL) {
        esp_ota_set_boot_partition(factory_partition);
        delay(500);
        ESP.restart(); // 强制重启，控制权交还给 Launcher
    } else {
        tft.setCursor(10, 80);
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.print("Launcher Not Found!");
    }
}

// ==========================================
// 初始化设置 (Setup)
// ==========================================
void setup() {
    Serial.begin(115200);
    delay(500);

    // 1. 初始化所有底层模块
    display_init();  
    buttons_init();  
    buzzer_init();   

    // 2. 初始化 SD 卡 (根据 pins.h，SD_CS 是 22)
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    
    if (!SD.begin(SD_CS)) {
        Serial.println("SD Card Mount Failed");
        display_show_info("Error", "SD Card", "Mount Failed");
    } else {
        Serial.println("SD Card OK");
        display_show_info("Tang Poetry", "Loading...", "SD OK");
    }

    delay(1000);
    display_clear();

    // ==========================================
    // 3. 此处可以初始化你的唐诗索引引擎与字体
    // u8g2Fonts.begin(tft);
    // loadTangPoetryIndex();
    // ==========================================
}

// ==========================================
// 主循环 (Loop)
// ==========================================
void loop() {
    // 使用高级防抖函数读取全局按键状态
    button_state_t btns = buttons_read_debounced();

    // ------------------------------------------
    // [退出逻辑]：根据 pins.h，BTN_LEFT 被映射到 27 号引脚
    // 我们将“向左键”作为退出到主菜单的快捷键
    // ------------------------------------------
    if (btns.left) {
        buzzer_beep();     // 退出前调用蜂鸣器发出短促的滴声
        returnToLauncher();
    }

    // ------------------------------------------
    // [翻页逻辑]：上下键进行诗词切换
    // ------------------------------------------
    if (btns.up) {
        // renderPreviousPoem();
        Serial.println("Previous Poem");
        delay(200); // 翻页基础防抖
    }

    if (btns.down) {
        // renderNextPoem();
        Serial.println("Next Poem");
        delay(200);
    }

    // ==========================================
    // 这里放置你的屏幕渲染刷新代码
    // ==========================================
    
    delay(30); // 降低 CPU 占用
}
