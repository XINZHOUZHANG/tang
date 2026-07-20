#include <Arduino.h>
#include <TFT_eSPI.h>
#include "display.h" // 核心修改：引入底层已经定义好的 tft 对象

// ==========================================
// 1. 硬件引脚定义 (沿用你掌机的底层配置)
// ==========================================
#define BTN_UP      2
#define BTN_DOWN    13
#define BTN_LEFT    27
#define BTN_RIGHT   35   // 注意: 仅输入引脚
#define BTN_A       34   // 注意: 仅输入引脚
#define BTN_B       12

// 【注意：这里原本的 TFT_eSPI tft = TFT_eSPI(); 已经被删掉了】

// ==========================================
// 2. 游戏全局变量
// ==========================================
// 战机的初始位置 (居中靠下)
int playerX = 60;
int playerY = 130;
const int playerWidth = 10;
const int playerHeight = 10;

// ... 后面的 setup() 和 loop() 代码完全保持不变 ...

// ==========================================
// 3. 初始化设置
// ==========================================
void setup() {
    Serial.begin(115200);

    // 初始化按键 (34, 35引脚需要硬件上拉，其他使用内部上拉)
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_LEFT, INPUT_PULLUP);
    pinMode(BTN_B, INPUT_PULLUP);
    pinMode(BTN_RIGHT, INPUT); 
    pinMode(BTN_A, INPUT);

    // 初始化屏幕
    tft.init();
    tft.setRotation(3); // 设为横屏模式，你可以根据实际手感改成 0 或 1
    tft.fillScreen(TFT_BLACK);
}

// ==========================================
// 4. 主循环 (游戏帧刷新)
// ==========================================
void loop() {
    // 【步骤 1】擦除战机之前的位置 (用黑色覆盖)
    tft.fillRect(playerX, playerY, playerWidth, playerHeight, TFT_BLACK);

    // 【步骤 2】读取按键，更新战机坐标 (每次移动 3 个像素)
    if (digitalRead(BTN_LEFT) == LOW)  playerX -= 3;
    if (digitalRead(BTN_RIGHT) == LOW) playerX += 3;
    if (digitalRead(BTN_UP) == LOW)    playerY -= 3;
    if (digitalRead(BTN_DOWN) == LOW)  playerY += 3;

    // 【步骤 3】边界限制 (防止战机飞出屏幕)
    if (playerX < 0) playerX = 0;
    if (playerX > tft.width() - playerWidth) playerX = tft.width() - playerWidth;
    if (playerY < 0) playerY = 0;
    if (playerY > tft.height() - playerHeight) playerY = tft.height() - playerHeight;

    // 【步骤 4】在新的坐标绘制战机 (绿色方块)
    tft.fillRect(playerX, playerY, playerWidth, playerHeight, TFT_GREEN);

    // 【步骤 5】控制游戏帧率 (大约 50 FPS)
    delay(20); 
}
