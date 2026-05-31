#ifndef GFX_H
#define GFX_H

#include <stdint.h>
#include <stdbool.h>

#define GFX_W       128
#define GFX_HEIGHT  160
#define GFX_FB_SIZE (GFX_W * GFX_HEIGHT / 2)

typedef uint8_t gfx_color_t;

#define GFX_NO_TRANSPARENT 0xFF

extern uint16_t gfx_palette[16];
extern uint8_t  gfx_fb[GFX_FB_SIZE];

void GFX_Init(void);

void GFX_Clear(gfx_color_t c);
void GFX_SetPixel(int x, int y, gfx_color_t c);
gfx_color_t GFX_GetPixel(int x, int y);

void GFX_FillRect(int x, int y, int w, int h, gfx_color_t c);
void GFX_DrawRect(int x, int y, int w, int h, gfx_color_t c);
void GFX_DrawHLine(int x, int y, int w, gfx_color_t c);
void GFX_DrawVLine(int x, int y, int h, gfx_color_t c);
void GFX_DrawLine(int x0, int y0, int x1, int y1, gfx_color_t c);
void GFX_DrawCircle(int cx, int cy, int r, gfx_color_t c);
void GFX_FillCircle(int cx, int cy, int r, gfx_color_t c);

void GFX_DrawChar(int x, int y, char ch, gfx_color_t fg, gfx_color_t bg, uint8_t size);
void GFX_DrawString(int x, int y, const char *s, gfx_color_t fg, gfx_color_t bg, uint8_t size);

void GFX_DrawSprite(int x, int y, int w, int h, const uint8_t *data,
                    gfx_color_t transparent);

void GFX_Flush(void);

#endif
