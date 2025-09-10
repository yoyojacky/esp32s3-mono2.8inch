#ifndef ST7305_H
#define ST7305_H

#include <Arduino.h>
#include <SPI.h>

#define LCD_RES 17
#define LCD_DC  16
#define LCD_CS  15
#define LCD_SCLK 7
#define LCD_SDI  6
#define TE_PIN   18


class ST7305 {
public:
    ST7305();
    void begin();
	/* 加入显存缓冲 + 文字功能 */
	void clearScreen(uint8_t gray = 0);   // 0-3 灰度
	void drawPixel(uint16_t x, uint16_t y, uint8_t gray);
	void puts8x8(uint16_t x, uint16_t y, const char *str, uint8_t gray);
	void flushFramebuffer();              // 把整屏缓冲刷给 LCD
    void showPicture(const uint8_t* pic);

	/* 屏幕尺寸常量 */
    static const uint16_t W = 168;
    static const uint16_t H = 384;

private:
    void spiWrite(uint8_t data);
    void writeRegister(uint8_t reg);
    void writeParameter(uint8_t param);
    void setAddress();
};

#endif
