#include "ST7305.h"
#include "pic.h"

ST7305 lcd;

void setup() {
  Serial.begin(115200);
  lcd.begin();
  //lcd.clearScreen(3);                     // 黑底
  //lcd.puts8x8(4, 4, "Hello ST7305!", 3);  // 最白
  //lcd.flushFramebuffer();                 // 一次刷屏
  lcd.showPicture(pic1);
}

void loop() {
  // 可添加刷新逻辑或 TE 中断处理
}
