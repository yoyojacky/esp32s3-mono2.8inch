#include "ST7305.h"
#include "font8x8.h"


static uint8_t fb[16128];  // 4-level, packed 4-pixel/byte

/* packed pixels */
static void packPixel(uint16_t idx, uint8_t gray) {
	uint8_t shift = (idx & 3) << 1;
  	uint8_t mask  = ~(0b11 << shift);
  	fb[idx >> 2] = (fb[idx >> 2] & mask) | (gray << shift);
}

void ST7305::clearScreen(uint8_t gray) {
  uint8_t pat = 0;
  for (uint8_t i = 0; i < 4; ++i) pat |= (gray << (i * 2));
  memset(fb, pat, sizeof(fb));
}

void ST7305::drawPixel(uint16_t x, uint16_t y, uint8_t gray) {
  if (x >= ST7305::W || y >= ST7305::H) return;
  packPixel(y * ST7305::W + x, gray & 0b11);
}

void ST7305::puts8x8(uint16_t x0, uint16_t y0, const char *str, uint8_t gray) {
  uint16_t x = x0, y = y0;
  for (const char *p = str; *p; ++p) {
    if (*p == '\n') { x = x0; y += 8; continue; }
    uint8_t ch = *p - 0x20;          // 字体从空格开始
    if (ch >= 96) continue;
    for (uint8_t row = 0; row < 8; ++row) {
      uint8_t bits = font8x8[ch][row];
      for (uint8_t col = 0; col < 8; ++col) {
        drawPixel(x + col, y + row, (bits >> (7 - col)) & 1 ? gray : 0);
      }
    }
    x += 8;
    if (x + 8 > ST7305::W) { x = x0; y += 8; }   // 自动换行
  }
}

void ST7305::flushFramebuffer() {
  setAddress();                      // 0,0 开始
  digitalWrite(LCD_DC, HIGH);
  digitalWrite(LCD_CS, LOW);
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(fb, 16128);
  SPI.endTransaction();
  digitalWrite(LCD_CS, HIGH);
}


ST7305::ST7305() {
    pinMode(LCD_RES, OUTPUT);
    pinMode(LCD_DC, OUTPUT);
    pinMode(LCD_CS, OUTPUT);
    SPI.begin(LCD_SCLK, -1, LCD_SDI, LCD_CS); // SCLK, MISO, MOSI, SS
}

void ST7305::spiWrite(uint8_t data) {
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    digitalWrite(LCD_CS, LOW);
    SPI.transfer(data);
    digitalWrite(LCD_CS, HIGH);
    SPI.endTransaction();
}

void ST7305::writeRegister(uint8_t reg) {
    digitalWrite(LCD_DC, LOW);
    spiWrite(reg);
}

void ST7305::writeParameter(uint8_t param) {
    digitalWrite(LCD_DC, HIGH);
    spiWrite(param);
}

void ST7305::begin() {
    digitalWrite(LCD_RES, HIGH);
    delay(10);
    digitalWrite(LCD_RES, LOW);
    delay(10);
    digitalWrite(LCD_RES, HIGH);
    delay(10);

    // 初始化寄存器序列（从文件1中提取）
    writeRegister(0xD6); writeParameter(0x17); writeParameter(0x02);
    writeRegister(0xD1); writeParameter(0x01);
    writeRegister(0xC0); writeParameter(0x08); writeParameter(0x06);
    writeRegister(0xC1); writeParameter(0x3C); writeParameter(0x3C); writeParameter(0x3C); writeParameter(0x3C);
    writeRegister(0xC2); writeParameter(0x23); writeParameter(0x23); writeParameter(0x23); writeParameter(0x23);
    writeRegister(0xC4); writeParameter(0x5A); writeParameter(0x5A); writeParameter(0x5A); writeParameter(0x5A);
    writeRegister(0xC5); writeParameter(0x37); writeParameter(0x37); writeParameter(0x37); writeParameter(0x37);
    writeRegister(0xD8); writeParameter(0xA6); writeParameter(0xE9);
    writeRegister(0xB2); writeParameter(0x12);
    writeRegister(0xB3); writeParameter(0xE5); writeParameter(0xF6); writeParameter(0x05); writeParameter(0x46); writeParameter(0x77); writeParameter(0x77); writeParameter(0x77); writeParameter(0x77); writeParameter(0x76); writeParameter(0x45);
    writeRegister(0xB4); writeParameter(0x05); writeParameter(0x46); writeParameter(0x77); writeParameter(0x77); writeParameter(0x77); writeParameter(0x77); writeParameter(0x76); writeParameter(0x45);
    writeRegister(0x62); writeParameter(0x32); writeParameter(0x03); writeParameter(0x1F);
    writeRegister(0xB7); writeParameter(0x13);
    writeRegister(0xB0); writeParameter(0x60);
    writeRegister(0x11); delay(255);
    writeRegister(0xC9); writeParameter(0x00);
    writeRegister(0x36); writeParameter(0x48);
    writeRegister(0x3A); writeParameter(0x10);
    writeRegister(0xB9); writeParameter(0x20);
    writeRegister(0xB8); writeParameter(0x09);
    writeRegister(0x2A); writeParameter(0x17); writeParameter(0x24);
    writeRegister(0x2B); writeParameter(0x00); writeParameter(0xBF);
    writeRegister(0x35); writeParameter(0x00);
    writeRegister(0xD0); writeParameter(0xFF);
    writeRegister(0x39);
    writeRegister(0x29);
}

void ST7305::setAddress() {
    writeRegister(0x2A); writeParameter(0x17); writeParameter(0x24);
    writeRegister(0x2B); writeParameter(0x00); writeParameter(0xBF);
    writeRegister(0x2C);
}
/*
void ST7305::setAddress() {
    writeRegister(0x2A);
	writeParameter(0x00); 
	writeParameter(0x9F);
    writeRegister(0x2B); 
	writeParameter(0x00); 
	writeParameter(0x1F);
    writeRegister(0x2C);
}
*/


void ST7305::showPicture(const uint8_t* pic) {
    setAddress();
    digitalWrite(LCD_DC, HIGH);
    digitalWrite(LCD_CS, LOW);
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    for (int i = 0; i < 192 * 14 * 4; i++) {
        SPI.transfer(pic[i]);
    }
    SPI.endTransaction();
    digitalWrite(LCD_CS, HIGH);
}
