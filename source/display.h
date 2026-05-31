#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "fsl_common.h"
#include <stdint.h>

// 屏幕分辨率（1.8" V1.2 通常是 128x160，红色 tab） 
#define ST7735_WIDTH   128
#define ST7735_HEIGHT  160

// 常用 RGB565 颜色
#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_YELLOW  0xFFE0
#define COLOR_CYAN    0x07FF
#define COLOR_MAGENTA 0xF81F

// 初始化 SPI + GPIO + ST7735
void Display_Init(void);

// 全屏填充
void Display_FillScreen(uint16_t color);

// 矩形填充
void Display_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

// 画单个像素
void Display_DrawPixel(uint16_t x, uint16_t y, uint16_t color);

// 画 8x8 ASCII 字符（前景色 + 背景色）
void Display_DrawChar(uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg);

// 画字符串
void Display_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg);

// GFX 层用：直接把一行 RGB565 像素送到屏幕 (y, len 像素)
void Display_BlitRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                      const uint16_t *rgb565);

// RGB888 → RGB565 辅助
static inline uint16_t Display_Color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

#endif // DISPLAY_H_
