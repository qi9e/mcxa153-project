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
 *   P3_12 -> CTIMER1_MAT2 (蜂鸣器 PWM 硬件输出)
 *
 * 注意: P1_7 (板载 SW2 按钮) 由 Button_Init() 独立配置, 不在这里
 */

#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

/* ============================================================
 * MCXA153 引脚复用号速查 (Alt 号)
 *   LPUART0_RXD  on P0_2  = ALT2
 *   LPUART0_TXD  on P0_3  = ALT2
 *   LPSPI1_SDO   on P2_13 = ALT2
 *   LPSPI1_SCK   on P2_12 = ALT2
 *   CTIMER1_MAT2 on P3_12 = ALT4  ← 请在 Config Tools 确认!
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
        kPORT_MuxAlt2,                      /* LPSPI1 = Alt2 */
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
    /* TFT 控制线 */
    PORT_SetPinConfig(PORT3, 15U, &gpio_out_cfg);  /* CS  */
    PORT_SetPinConfig(PORT3, 14U, &gpio_out_cfg);  /* RST */
    PORT_SetPinConfig(PORT2,  6U, &gpio_out_cfg);  /* DC  */
    /* 键盘行 */
    PORT_SetPinConfig(PORT1, 10U, &gpio_out_cfg);  /* R1 */
    PORT_SetPinConfig(PORT1, 12U, &gpio_out_cfg);  /* R2 */
    PORT_SetPinConfig(PORT1, 13U, &gpio_out_cfg);  /* R3 */
    PORT_SetPinConfig(PORT2,  0U, &gpio_out_cfg);  /* R4 */

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
    PORT_SetPinConfig(PORT3, 31U, &gpio_in_cfg);   /* C1 */
    PORT_SetPinConfig(PORT3, 30U, &gpio_in_cfg);   /* C2 */
    PORT_SetPinConfig(PORT3, 29U, &gpio_in_cfg);   /* C3 */
    PORT_SetPinConfig(PORT1, 11U, &gpio_in_cfg);   /* C4 */

    /* ====== 8. GPIO 方向 + 初始电平 ====== */
    const gpio_pin_config_t out_high = { kGPIO_DigitalOutput, 1 };
    const gpio_pin_config_t in_cfg   = { kGPIO_DigitalInput,  0 };

    /* TFT 控制线: CS/RST 高电平待命, DC 高 */
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

    /* ====== 9. 蜂鸣器: P3_12 -> CTIMER1_MAT2 (硬件 PWM 输出) ====== */
    /*
     * 不再用 GPIO + 中断翻转, 改为 CTIMER 硬件 MAT 输出
     * CTIMER 直接驱动引脚, 不需要 GPIO_PinInit
     *
     * ALT 号: 请在 MCUXpresso Config Tools 中确认
     *   打开 Pins 视图 -> 搜索 P3_12 -> 选 CTIMER1:MATCH,2
     *   查看生成代码里 kPORT_MuxAlt 后面的数字
     *   下面暂写 kPORT_MuxAlt4, 如不对请替换
     */
    const port_pin_config_t ctimer_out_cfg = {
        kPORT_PullDisable,
        kPORT_LowPullResistor,
        kPORT_FastSlewRate,
        kPORT_PassiveFilterDisable,
        kPORT_OpenDrainDisable,
        kPORT_LowDriveStrength,
        kPORT_NormalDriveStrength,
        kPORT_MuxAlt4,                      /* CTIMER1_MAT2, 请确认 ALT 号! */
        kPORT_InputBufferEnable,
        kPORT_InputNormal,
        kPORT_UnlockRegister
    };
    PORT_SetPinConfig(PORT3, 12U, &ctimer_out_cfg);
    /* 无 GPIO_PinInit — CTIMER 硬件直接驱动 MAT 引脚 */
}
