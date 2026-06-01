/*
 * 
 *
 * 
 *   P0_2  -> LPUART0_RXD  (debug)
 *   P0_3  -> LPUART0_TXD  (debug)
 *   P2_13 -> LPSPI1_SDO   (TFT MOSI)
 *   P2_12 -> LPSPI1_SCK   (TFT SCK)
 *   P3_15 -> GPIO         (TFT CS)
 *   P3_14 -> GPIO         (TFT RST)
 *   P2_6  -> GPIO         (TFT DC/A0)
 *   P1_10, P1_12, P1_13, P2_0  -> GPIO out  (rows of keypad)
 *   P3_31, P3_30, P3_29, P1_11 -> GPIO in  (columns of keypad)
 *   P3_12 -> CTIMER1_MAT2 (beezer PWM hardware output)
 *
 * atencion: P1_7 (sw2 button) is Button_Init() alone,
 */

#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

/* ============================================================
 * 
 *   LPUART0_RXD  on P0_2  = ALT2
 *   LPUART0_TXD  on P0_3  = ALT2
 *   LPSPI1_SDO   on P2_13 = ALT2
 *   LPSPI1_SCK   on P2_12 = ALT2
 *   CTIMER1_MAT2 on P3_12 = ALT4
 *   
 * ============================================================ */

void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
}

void BOARD_InitPins(void)
{
    /* ====== 1. enable clocks ====== */
    CLOCK_EnableClock(kCLOCK_GatePORT0);
    CLOCK_EnableClock(kCLOCK_GatePORT1);
    CLOCK_EnableClock(kCLOCK_GatePORT2);
    CLOCK_EnableClock(kCLOCK_GatePORT3);

    /* ====== 2. enable all used GPIO clocks ====== */
    CLOCK_EnableClock(kCLOCK_GateGPIO0);
    CLOCK_EnableClock(kCLOCK_GateGPIO1);
    CLOCK_EnableClock(kCLOCK_GateGPIO2);
    CLOCK_EnableClock(kCLOCK_GateGPIO3);

    /* ====== 3. reset peripherals ====== */
    RESET_ReleasePeripheralReset(kPORT0_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kPORT1_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kPORT2_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kPORT3_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO0_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO1_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO2_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO3_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kLPUART0_RST_SHIFT_RSTn);

    /* ====== 4. LPUART0 pin (P0_2 RX, P0_3 TX) ====== */
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

    /* ====== 5. LPSPI1 pin (P2_13 SDO, P2_12 SCK) ====== */
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

    /* ====== 6. GPIO output pins (TFT control signals + keypad rows) ====== */
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
    /* TFT control signals */
    PORT_SetPinConfig(PORT3, 15U, &gpio_out_cfg);  /* CS  */
    PORT_SetPinConfig(PORT3, 14U, &gpio_out_cfg);  /* RST */
    PORT_SetPinConfig(PORT2,  6U, &gpio_out_cfg);  /* DC  */
    /* keypad rows */
    PORT_SetPinConfig(PORT1, 10U, &gpio_out_cfg);  /* R1 */
    PORT_SetPinConfig(PORT1, 12U, &gpio_out_cfg);  /* R2 */
    PORT_SetPinConfig(PORT1, 13U, &gpio_out_cfg);  /* R3 */
    PORT_SetPinConfig(PORT2,  0U, &gpio_out_cfg);  /* R4 */

    /* ====== 7. GPIO input pins (keypad columns with pull-up) ====== */
    const port_pin_config_t gpio_in_cfg = {
        kPORT_PullUp,                       /* columns default to pull-up, rows scanned by pulling low */
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

    /* ====== 8. GPIO direction + initial state ====== */
    const gpio_pin_config_t out_high = { kGPIO_DigitalOutput, 1 };
    const gpio_pin_config_t in_cfg   = { kGPIO_DigitalInput,  0 };

    /* TFT control signals: CS/RST high, DC high */
    GPIO_PinInit(GPIO3, 15U, &out_high);   /* CS  */
    GPIO_PinInit(GPIO3, 14U, &out_high);   /* RST */
    GPIO_PinInit(GPIO2,  6U, &out_high);   /* DC  */

    /* Keypad rows: default high, scanned by pulling low */
    GPIO_PinInit(GPIO1, 10U, &out_high);
    GPIO_PinInit(GPIO1, 12U, &out_high);
    GPIO_PinInit(GPIO1, 13U, &out_high);
    GPIO_PinInit(GPIO2,  0U, &out_high);

    /* Keypad columns: input */
    GPIO_PinInit(GPIO3, 31U, &in_cfg);
    GPIO_PinInit(GPIO3, 30U, &in_cfg);
    GPIO_PinInit(GPIO3, 29U, &in_cfg);
    GPIO_PinInit(GPIO1, 11U, &in_cfg);

    /* ====== 9. buzzer: P3_12 -> CTIMER1_MAT2 harware pwm ====== */
    /*
     * 
     */
    const port_pin_config_t ctimer_out_cfg = {
        kPORT_PullDisable,
        kPORT_LowPullResistor,
        kPORT_FastSlewRate,
        kPORT_PassiveFilterDisable,
        kPORT_OpenDrainDisable,
        kPORT_LowDriveStrength,
        kPORT_NormalDriveStrength,
        kPORT_MuxAlt4,                      
        kPORT_InputBufferEnable,
        kPORT_InputNormal,
        kPORT_UnlockRegister
    };
    PORT_SetPinConfig(PORT3, 12U, &ctimer_out_cfg);
}
