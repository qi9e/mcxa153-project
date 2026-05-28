/*
 * pin_mux.c - 手写引脚配置, 不依赖 Config Tools
 *
 * 配置:
 *   P0_2  -> LPUART0_RXD  (调试串口)
 *   P0_3  -> LPUART0_TXD  (调试串口)
 *   P2_13 -> LPSPI1_SDO   (TFT MOSI)
 *   P2_12 -> LPSPI1_SCK   (TFT SCK)
 *   P3_15 -> GPIO         (TFT CS)
 *   P3_14 -> GPIO         (TFT RST)
 *   P2_6  -> GPIO         (TFT DC/A0)
 *   P1_10, P1_12, P1_13, P2_0  -> GPIO 输出  (键盘行)
 *   P3_31, P3_30, P3_29, P1_11 -> GPIO 输入  (键盘列)
 */

#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

/* ============================================================
 * MCXA153 引脚复用号速查 (Alt 号)
 * 关键的几个:
 *   LPUART0_RXD on P0_2  = ALT2
 *   LPUART0_TXD on P0_3  = ALT2
 *   LPSPI1_SDO  on P2_13 = ALT2   (推测, 跟 Config Tools 选时是 ALT2)
 *   LPSPI1_SCK  on P2_12 = ALT2   (同上)
 *   GPIO 在所有引脚上都是 ALT0
 * ============================================================ */

void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
}

void BOARD_InitPins(void)
{
    /* ====== 1. 开所有用到的 PORT 时钟门 ====== */
    CLOCK_EnableClock(kCLOCK_GatePORT0);
    CLOCK_EnableClock(kCLOCK_GatePORT1);
    CLOCK_EnableClock(kCLOCK_GatePORT2);
    CLOCK_EnableClock(kCLOCK_GatePORT3);

    /* ====== 2. 开所有用到的 GPIO 时钟门 ====== */
    CLOCK_EnableClock(kCLOCK_GateGPIO0);
    CLOCK_EnableClock(kCLOCK_GateGPIO1);
    CLOCK_EnableClock(kCLOCK_GateGPIO2);
    CLOCK_EnableClock(kCLOCK_GateGPIO3);

    /* ====== 3. 释放外设复位 ====== */
    RESET_ReleasePeripheralReset(kPORT0_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kPORT1_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kPORT2_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kPORT3_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO0_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO1_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO2_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO3_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kLPUART0_RST_SHIFT_RSTn);

    /* ====== 4. LPUART0 引脚 (P0_2 RX, P0_3 TX) ====== */
    const port_pin_config_t uart_cfg = {
        kPORT_PullUp,
        kPORT_LowPullResistor,
        kPORT_FastSlewRate,
        kPORT_PassiveFilterDisable,
        kPORT_OpenDrainDisable,
        kPORT_LowDriveStrength,
        kPORT_NormalDriveStrength,
        kPORT_MuxAlt2,                      /* LPUART0 = Alt2 */
        kPORT_InputBufferEnable,
        kPORT_InputNormal,
        kPORT_UnlockRegister
    };
    PORT_SetPinConfig(PORT0, 2U, &uart_cfg);
    PORT_SetPinConfig(PORT0, 3U, &uart_cfg);

    /* ====== 5. LPSPI1 引脚 (P2_13 SDO, P2_12 SCK) ====== */
    const port_pin_config_t spi_cfg = {
        kPORT_PullDisable,
        kPORT_LowPullResistor,
        kPORT_FastSlewRate,
        kPORT_PassiveFilterDisable,
        kPORT_OpenDrainDisable,
        kPORT_LowDriveStrength,
        kPORT_NormalDriveStrength,
        kPORT_MuxAlt2,                      /* LPSPI1 = Alt2 (推测) */
        kPORT_InputBufferEnable,
        kPORT_InputNormal,
        kPORT_UnlockRegister
    };
    PORT_SetPinConfig(PORT2, 13U, &spi_cfg);
    PORT_SetPinConfig(PORT2, 12U, &spi_cfg);

    /* ====== 6. GPIO 输出引脚 (TFT 控制信号 + 键盘行) ====== */
    const port_pin_config_t gpio_out_cfg = {
        kPORT_PullDisable,
        kPORT_LowPullResistor,
        kPORT_FastSlewRate,
        kPORT_PassiveFilterDisable,
        kPORT_OpenDrainDisable,
        kPORT_LowDriveStrength,
        kPORT_NormalDriveStrength,
        kPORT_MuxAlt0,                      /* GPIO = Alt0 */
        kPORT_InputBufferEnable,
        kPORT_InputNormal,
        kPORT_UnlockRegister
    };
    /* TFT */
    PORT_SetPinConfig(PORT3, 15U, &gpio_out_cfg);  /* CS  */
    PORT_SetPinConfig(PORT3, 14U, &gpio_out_cfg);  /* RST */
    PORT_SetPinConfig(PORT2,  6U, &gpio_out_cfg);  /* DC  */
    /* Keypad 行 */
    PORT_SetPinConfig(PORT1, 10U, &gpio_out_cfg);  /* t1 */
    PORT_SetPinConfig(PORT1, 12U, &gpio_out_cfg);  /* t2 */
    PORT_SetPinConfig(PORT1, 13U, &gpio_out_cfg);  /* t3 */
    PORT_SetPinConfig(PORT2,  0U, &gpio_out_cfg);  /* t4 */

    /* ====== 7. GPIO 输入引脚 (键盘列, 带上拉) ====== */
    const port_pin_config_t gpio_in_cfg = {
        kPORT_PullUp,                       /* 列默认上拉, 行扫描拉低读 */
        kPORT_LowPullResistor,
        kPORT_FastSlewRate,
        kPORT_PassiveFilterDisable,
        kPORT_OpenDrainDisable,
        kPORT_LowDriveStrength,
        kPORT_NormalDriveStrength,
        kPORT_MuxAlt0,                      /* GPIO */
        kPORT_InputBufferEnable,
        kPORT_InputNormal,
        kPORT_UnlockRegister
    };
    PORT_SetPinConfig(PORT3, 31U, &gpio_in_cfg);   /* ti1 */
    PORT_SetPinConfig(PORT3, 30U, &gpio_in_cfg);   /* ti2 */
    PORT_SetPinConfig(PORT3, 29U, &gpio_in_cfg);   /* ti3 */
    PORT_SetPinConfig(PORT1, 11U, &gpio_in_cfg);   /* ti4 */

    /* ====== 8. GPIO 方向 + 初始电平 ====== */
    const gpio_pin_config_t out_high = { kGPIO_DigitalOutput, 1 };
    const gpio_pin_config_t out_low  = { kGPIO_DigitalOutput, 0 };
    const gpio_pin_config_t in_cfg   = { kGPIO_DigitalInput,  0 };

    /* ====== 9. Buzzer 引脚 (P3_8, GPIO 输出, 由 CTIMER 中断翻转) ====== */
	PORT_SetPinConfig(PORT3, 8U, &gpio_out_cfg);
	GPIO_PinInit(GPIO3, 8U, &out_low);

    /* TFT 控制线初始电平: CS/RST 高电平待命, DC 高 */
    GPIO_PinInit(GPIO3, 15U, &out_high);   /* CS  */
    GPIO_PinInit(GPIO3, 14U, &out_high);   /* RST */
    GPIO_PinInit(GPIO2,  6U, &out_high);   /* DC  */

    /* 键盘行: 默认拉高, 扫描时逐个拉低 */
    GPIO_PinInit(GPIO1, 10U, &out_high);
    GPIO_PinInit(GPIO1, 12U, &out_high);
    GPIO_PinInit(GPIO1, 13U, &out_high);
    GPIO_PinInit(GPIO2,  0U, &out_high);

    /* 键盘列: 输入 */
    GPIO_PinInit(GPIO3, 31U, &in_cfg);
    GPIO_PinInit(GPIO3, 30U, &in_cfg);
    GPIO_PinInit(GPIO3, 29U, &in_cfg);
    GPIO_PinInit(GPIO1, 11U, &in_cfg);
}
