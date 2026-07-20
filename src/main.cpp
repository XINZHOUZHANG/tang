#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <U8g2_for_Adafruit_GFX.h>

// ================= 引脚与硬件配置 =================
#define TFT_CS      5
#define TFT_DC      4
#define TFT_RST     19
#define SD_CS       22
#define BUZZER_PIN  14

// 按键引脚定义 (注意 34和35 仅支持输入，需外部上拉或硬件电路支持)
#define BTN_UP      2
#define BTN_DOWN    13
#define BTN_LEFT    27
#define BTN_RIGHT   35
#define BTN_A       34
#define BTN_B       12

// 颜色定义补全 (使用你之前修复的灰色)
#define COLOR_DARKGREY 0x7BEF 

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

// ================= 诗词索引引擎 =================
const int MAX_POEMS = 350;       // 最多支持缓存 350 首诗的索引
uint32_t poemOffsets[MAX_POEMS]; // 保存每首诗在 TXT 文件中的起始字节位置
int totalPoems = 0;              // 实际读取到的诗词总数
int currentPoemIndex = 0;        // 当前正在阅读的诗词序号

// 按键防抖变量
unsigned long lastButtonPress = 0;
const int DEBOUNCE_DELAY = 200;

// ================= 函数声明 =================
void buildPoemIndex();
void displayPoem(int index);
void beep();

void setup() {
  Serial.begin(115200);

  // 初始化蜂鸣器
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // 初始化按键 (带内部上拉的引脚)
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  // 34, 35 仅能配置为 INPUT
  pinMode(BTN_RIGHT, INPUT);
  pinMode(BTN_A, INPUT);

  // 初始化屏幕
  tft.initR(INITR_REDTAB);
  tft.setRotation(3); // 横屏显示
  tft.fillScreen(ST77XX_BLACK);

  // 初始化中文字库引擎
  u8g2Fonts.begin(tft);
  u8g2Fonts.setFont(u8g2_font_wqy16_t_chinese1);
  u8g2Fonts.setFontMode(1);
  u8g2Fonts.setFontDirection(0);

  // 绘制开机 Splash
  u8g2Fonts.setForegroundColor(ST77XX_WHITE);
  u8g2Fonts.setCursor(30, 60);
  u8g2Fonts.print("系统初始化...");

  // 初始化 SD 卡
  if (!SD.begin(SD_CS)) {
    tft.fillScreen(ST77XX_BLACK);
    u8g2Fonts.setForegroundColor(ST77XX_RED);
    u8g2Fonts.setCursor(20, 60);
    u8g2Fonts.print("SD卡挂载失败!");
    while (true); // 死循环停机
  }

  // 扫描并建立唐诗索引
  u8g2Fonts.setCursor(30, 80);
  u8g2Fonts.print("正在建立索引...");
  buildPoemIndex();

  if (totalPoems == 0) {
    tft.fillScreen(ST77XX_BLACK);
    u8g2Fonts.setForegroundColor(ST77XX_RED);
    u8g2Fonts.setCursor(10, 60);
    u8g2Fonts.print("未找到 tang.txt 文件");
    while (true);
  }

  // 开机完成，发出提示音并显示第一首诗
  beep();
  displayPoem(currentPoemIndex);
}

void loop() {
  // 读取按键 (低电平触发)
  bool upPressed = (digitalRead(BTN_UP) == LOW) || (digitalRead(BTN_LEFT) == LOW);
  bool downPressed = (digitalRead(BTN_DOWN) == LOW) || (digitalRead(BTN_RIGHT) == LOW);

  if (millis() - lastButtonPress > DEBOUNCE_DELAY) {
    if (downPressed) {
      if (currentPoemIndex < totalPoems - 1) {
        currentPoemIndex++;
        displayPoem(currentPoemIndex);
        beep();
      }
      lastButtonPress = millis();
    } 
    else if (upPressed) {
      if (currentPoemIndex > 0) {
        currentPoemIndex--;
        displayPoem(currentPoemIndex);
        beep();
      }
      lastButtonPress = millis();
    }
  }
}

// ================= 核心逻辑：建立索引 =================
// 规则：只要遇到空行（连续的换行符），就认为下一行是一首新诗的开始
void buildPoemIndex() {
  File file = SD.open("/tang.txt", FILE_READ);
  if (!file) return;

  totalPoems = 0;
  poemOffsets[totalPoems++] = 0; // 第一首诗的起点一定是文件头部

  bool lastWasNewline = false;
  bool inBlankSpace = false;

  while (file.available()) {
    char c = file.read();
    if (c == '\n') {
      if (lastWasNewline) inBlankSpace = true; // 连续两个换行，说明是空行
      lastWasNewline = true;
    } else if (c == '\r') {
      // 忽略 Windows 系统的回车符
    } else {
      // 遇到正常字符
      if (inBlankSpace) {
        // 如果之前是在空行里，说明现在这个字符是新一首诗的开头
        if (totalPoems < MAX_POEMS) {
          poemOffsets[totalPoems++] = file.position() - 1; 
        }
        inBlankSpace = false;
      }
      lastWasNewline = false;
    }
  }
  file.close();
  Serial.printf("建立索引完成，共找到 %d 首诗\n", totalPoems);
}

// ================= 核心逻辑：渲染诗词 =================
void displayPoem(int index) {
  if (index < 0 || index >= totalPoems) return;

  tft.fillScreen(ST77XX_BLACK);
  File file = SD.open("/tang.txt", FILE_READ);
  if (!file) return;

  // 1. 光标直接跳到这首诗的起点
  file.seek(poemOffsets[index]);
  
  int y = 20; // 初始行高
  int lineCount = 0;

  // 2. 逐行读取并渲染，直到遇到空行或屏幕填满
  while (file.available() && y < 120) {
    String line = file.readStringUntil('\n');
    line.trim(); // 清除行尾的空格和不可见字符

    if (line.length() == 0) {
      break; // 遇到空行，说明这首诗结束了
    }

    // 诗词高亮逻辑：第一行通常是标题，第二行是作者
    if (lineCount == 0) {
      u8g2Fonts.setForegroundColor(ST77XX_YELLOW); // 标题黄色
    } else if (lineCount == 1) {
      u8g2Fonts.setForegroundColor(ST77XX_CYAN);   // 作者青色
    } else {
      u8g2Fonts.setForegroundColor(ST77XX_WHITE);  // 正文白色
    }

    u8g2Fonts.setCursor(10, y);
    u8g2Fonts.print(line);

    y += 20; // 行距设置 (字体16px + 间距4px)
    lineCount++;
  }
  file.close();

  // 3. 在右下角绘制页码进度
  u8g2Fonts.setForegroundColor(COLOR_DARKGREY);
  u8g2Fonts.setCursor(100, 124);
  u8g2Fonts.print(String(index + 1) + "/" + String(totalPoems));
}

// 蜂鸣器提示音
void beep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(20);
  digitalWrite(BUZZER_PIN, LOW);
}
