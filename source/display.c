#include "display.h"
#include "fsl_clock.h"
#include "fsl_gpio.h"
#include "fsl_lpspi.h"

#define PIN_CS_GPIO   GPIO3
#define PIN_CS_NUM    15U

#define PIN_RST_GPIO  GPIO3
#define PIN_RST_NUM   14U

#define PIN_DC_GPIO   GPIO2
#define PIN_DC_NUM    6U


#define DISPLAY_SPI            LPSPI1                           
#define DISPLAY_SPI_CLK_FREQ   CLOCK_GetLpspiClkFreq(1U)        

#define DISPLAY_SPI_BAUDRATE   6000000U   

#define CS_LOW()   GPIO_PinWrite(PIN_CS_GPIO,  PIN_CS_NUM, 0)
#define CS_HIGH()  GPIO_PinWrite(PIN_CS_GPIO,  PIN_CS_NUM, 1)
#define RST_LOW()  GPIO_PinWrite(PIN_RST_GPIO, PIN_RST_NUM, 0)
#define RST_HIGH() GPIO_PinWrite(PIN_RST_GPIO, PIN_RST_NUM, 1)
#define DC_CMD()   GPIO_PinWrite(PIN_DC_GPIO,  PIN_DC_NUM, 0)
#define DC_DATA()  GPIO_PinWrite(PIN_DC_GPIO,  PIN_DC_NUM, 1)

#define ST7735_SWRESET 0x01
#define ST7735_SLPOUT  0x11
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_MADCTL  0x36
#define ST7735_COLMOD  0x3A
#define ST7735_FRMCTR1 0xB1
#define ST7735_INVCTR  0xB4
#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_VMCTR1  0xC5
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

static void delay_ms(uint32_t ms)
{
    volatile uint32_t i;
    for (; ms > 0; ms--) for (i = 0; i < 12000; i++) __NOP();
}

static void spi_write_buf(const uint8_t *buf, uint32_t len)
{
    lpspi_transfer_t xfer = {0};
    xfer.txData      = (uint8_t *)buf;
    xfer.rxData      = NULL;
    xfer.dataSize    = len;
    xfer.configFlags = kLPSPI_MasterPcs0;
    LPSPI_MasterTransferBlocking(DISPLAY_SPI, &xfer);
}

static void spi_write_byte(uint8_t b)
{
    spi_write_buf(&b, 1);
}

static void write_cmd(uint8_t cmd)
{
    DC_CMD();  CS_LOW();
    spi_write_byte(cmd);
    CS_HIGH();
}

static void write_data(uint8_t d)
{
    DC_DATA(); CS_LOW();
    spi_write_byte(d);
    CS_HIGH();
}

static void write_data_buf(const uint8_t *buf, uint32_t len)
{
    DC_DATA(); CS_LOW();
    spi_write_buf(buf, len);
    CS_HIGH();
}

static void set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t d[4];
    write_cmd(ST7735_CASET);
    d[0] = 0; d[1] = x0; d[2] = 0; d[3] = x1;
    write_data_buf(d, 4);

    write_cmd(ST7735_RASET);
    d[0] = 0; d[1] = y0; d[2] = 0; d[3] = y1;
    write_data_buf(d, 4);

    write_cmd(ST7735_RAMWR);
}

static void gpio_init(void)
{
    CLOCK_EnableClock(kCLOCK_GateGPIO2);
    CLOCK_EnableClock(kCLOCK_GateGPIO3);
    CLOCK_EnableClock(kCLOCK_GatePORT2);
    CLOCK_EnableClock(kCLOCK_GatePORT3);

    const gpio_pin_config_t out_high = { kGPIO_DigitalOutput, 1 };
    GPIO_PinInit(GPIO3, 15U, &out_high);
    GPIO_PinInit(GPIO3, 14U, &out_high);
    //GPIO_PinInit(GPIO2,  6U, &out_high);
}

static void spi_init(void)
{
    CLOCK_EnableClock(kCLOCK_GateLPSPI1);                      

    RESET_ReleasePeripheralReset(kLPSPI1_RST_SHIFT_RSTn);

    CLOCK_SetClockDiv(kCLOCK_DivLPSPI1, 1U);
    CLOCK_AttachClk(kFRO12M_to_LPSPI1);

    lpspi_master_config_t cfg;
    LPSPI_MasterGetDefaultConfig(&cfg);
    cfg.baudRate     = DISPLAY_SPI_BAUDRATE;
    cfg.bitsPerFrame = 8U;
    cfg.cpol         = kLPSPI_ClockPolarityActiveHigh;
    cfg.cpha         = kLPSPI_ClockPhaseFirstEdge;
    cfg.direction    = kLPSPI_MsbFirst;
    cfg.whichPcs     = kLPSPI_Pcs0;
    cfg.pcsActiveHighOrLow = kLPSPI_PcsActiveLow;

    LPSPI_MasterInit(DISPLAY_SPI, &cfg, DISPLAY_SPI_CLK_FREQ);
}

static void st7735_init_seq(void)
{
	RST_HIGH(); delay_ms(50);
	RST_LOW();  delay_ms(50);
	RST_HIGH(); delay_ms(200);   /* 原来 120ms, 加到 200 */

	write_cmd(ST7735_SWRESET); delay_ms(200);   /* 原来 150 */
	write_cmd(ST7735_SLPOUT);  delay_ms(200);   /* 原来 120 */

    write_cmd(ST7735_FRMCTR1);
    { uint8_t d[] = {0x01, 0x2C, 0x2D}; write_data_buf(d, 3); }

    write_cmd(ST7735_INVCTR);  write_data(0x07);

    write_cmd(ST7735_PWCTR1);
    { uint8_t d[] = {0xA2, 0x02, 0x84}; write_data_buf(d, 3); }
    write_cmd(ST7735_PWCTR2);  write_data(0xC5);

    write_cmd(ST7735_VMCTR1);
    { uint8_t d[] = {0x0E, 0x14}; write_data_buf(d, 2); }

    write_cmd(ST7735_MADCTL);  write_data(0xC0);
    write_cmd(ST7735_COLMOD);  write_data(0x05);

    write_cmd(ST7735_GMCTRP1);
    { uint8_t d[] = {0x02,0x1c,0x07,0x12,0x37,0x32,0x29,0x2d,
                     0x29,0x25,0x2B,0x39,0x00,0x01,0x03,0x10};
      write_data_buf(d, 16); }
    write_cmd(ST7735_GMCTRN1);
    { uint8_t d[] = {0x03,0x1d,0x07,0x06,0x2E,0x2C,0x29,0x2D,
                     0x2E,0x2E,0x37,0x3F,0x00,0x00,0x02,0x10};
      write_data_buf(d, 16); }

    write_cmd(ST7735_DISPON);  delay_ms(100);
}

void Display_Init(void)
{
	gpio_init();
    spi_init();
    st7735_init_seq();
}

void Display_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
    if (x + w > ST7735_WIDTH)  w = ST7735_WIDTH  - x;
    if (y + h > ST7735_HEIGHT) h = ST7735_HEIGHT - y;

    set_addr_window(x, y, x + w - 1, y + h - 1);

    uint8_t row[ST7735_WIDTH * 2];
    uint8_t hi = color >> 8, lo = color & 0xFF;
    for (uint32_t i = 0; i < w; i++) {
        row[i * 2]     = hi;
        row[i * 2 + 1] = lo;
    }

    DC_DATA(); CS_LOW();
    for (uint32_t r = 0; r < h; r++) {
        spi_write_buf(row, w * 2);
    }
    CS_HIGH();
}

void Display_FillScreen(uint16_t color)
{
    Display_FillRect(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void Display_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
    set_addr_window(x, y, x, y);
    uint8_t d[2] = { color >> 8, color & 0xFF };
    write_data_buf(d, 2);
}

typedef struct { char c; uint8_t cols[5]; } font_glyph_t;

static const font_glyph_t g_font[] = {
    {' ', {0x00,0x00,0x00,0x00,0x00}},
    {'0', {0x3E,0x51,0x49,0x45,0x3E}},
    {'1', {0x00,0x42,0x7F,0x40,0x00}},
    {'2', {0x42,0x61,0x51,0x49,0x46}},
    {'3', {0x21,0x41,0x45,0x4B,0x31}},
    {'4', {0x18,0x14,0x12,0x7F,0x10}},
    {'5', {0x27,0x45,0x45,0x45,0x39}},
    {'6', {0x3C,0x4A,0x49,0x49,0x30}},
    {'7', {0x01,0x71,0x09,0x05,0x03}},
    {'8', {0x36,0x49,0x49,0x49,0x36}},
    {'9', {0x06,0x49,0x49,0x29,0x1E}},
    {'A', {0x7E,0x11,0x11,0x11,0x7E}},
    {'B', {0x7F,0x49,0x49,0x49,0x36}},
    {'C', {0x3E,0x41,0x41,0x41,0x22}},
    {'D', {0x7F,0x41,0x41,0x22,0x1C}},
    {'E', {0x7F,0x49,0x49,0x49,0x41}},
    {'F', {0x7F,0x09,0x09,0x09,0x01}},
    {'*', {0x14,0x08,0x3E,0x08,0x14}},
    {'#', {0x14,0x7F,0x14,0x7F,0x14}},
    {':', {0x00,0x36,0x36,0x00,0x00}},
    {'.', {0x00,0x60,0x60,0x00,0x00}},
    {'-', {0x08,0x08,0x08,0x08,0x08}},
};
#define FONT_COUNT (sizeof(g_font) / sizeof(g_font[0]))

void Display_DrawChar(uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg)
{
    /* 查表 */
    const uint8_t *cols = NULL;
    for (uint32_t i = 0; i < FONT_COUNT; i++) {
        if (g_font[i].c == c) { cols = g_font[i].cols; break; }
    }
    if (cols == NULL) cols = g_font[0].cols;  /* 没找到 → 空格 */

    uint8_t buf[6 * 8 * 2];
    uint8_t hi_fg = fg >> 8, lo_fg = fg & 0xFF;
    uint8_t hi_bg = bg >> 8, lo_bg = bg & 0xFF;

    uint32_t idx = 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 6; col++) {
            uint8_t pixel_on = 0;
            if (col < 5 && row < 7) {
                pixel_on = (cols[col] >> row) & 1;
            }
            if (pixel_on) {
                buf[idx++] = hi_fg;
                buf[idx++] = lo_fg;
            } else {
                buf[idx++] = hi_bg;
                buf[idx++] = lo_bg;
            }
        }
    }

    set_addr_window(x, y, x + 5, y + 7);
    DC_DATA(); CS_LOW();
    spi_write_buf(buf, sizeof(buf));
    CS_HIGH();
}

void Display_DrawString(uint16_t x, uint16_t y, const char *s, uint16_t fg, uint16_t bg)
{
    while (*s) {
        Display_DrawChar(x, y, *s++, fg, bg);
        x += 6;

        if (x + 6 > ST7735_WIDTH) break;
    }
}

void Display_BlitRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                      const uint16_t *rgb565)
{
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
    if (x + w > ST7735_WIDTH)  w = ST7735_WIDTH  - x;
    if (y + h > ST7735_HEIGHT) h = ST7735_HEIGHT - y;

    set_addr_window(x, y, x + w - 1, y + h - 1);
    DC_DATA(); CS_LOW();
   
    static uint8_t line[ST7735_WIDTH * 2];
    for (uint32_t r = 0; r < h; r++) {
        for (uint32_t i = 0; i < w; i++) {
            uint16_t c = rgb565[r * w + i];
            line[i * 2]     = c >> 8;
            line[i * 2 + 1] = c & 0xFF;
        }
        spi_write_buf(line, w * 2);
    }
    CS_HIGH();
}
