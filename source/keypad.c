/*
 * keypad.c - 4x4 软膜键盘扫描驱动 (MCXA153)
 */

#include "keypad.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_clock.h"
#include "fsl_reset.h"

/* ---------- 引脚定义 ---------- */
#define ROW0_PORT   PORT1
#define ROW0_GPIO   GPIO1
#define ROW0_PIN    10U   /* t1 -> P1_10 */

#define ROW1_PORT   PORT1
#define ROW1_GPIO   GPIO1
#define ROW1_PIN    12U   /* t2 -> P1_12 */

#define ROW2_PORT   PORT1
#define ROW2_GPIO   GPIO1
#define ROW2_PIN    13U   /* t3 -> P1_13 */

#define ROW3_PORT   PORT2
#define ROW3_GPIO   GPIO2
#define ROW3_PIN    0U    /* t4 -> P2_0 */

#define COL0_PORT   PORT3
#define COL0_GPIO   GPIO3
#define COL0_PIN    31U   /* ti1 -> P3_31 */

#define COL1_PORT   PORT3
#define COL1_GPIO   GPIO3
#define COL1_PIN    30U   /* ti2 -> P3_30 */

#define COL2_PORT   PORT3
#define COL2_GPIO   GPIO3
#define COL2_PIN    29U   /* ti3 -> P3_29 */

#define COL3_PORT   PORT1
#define COL3_GPIO   GPIO1
#define COL3_PIN    11U   /* ti4 -> P1_11 */

/* ---------- 键位映射 ---------- */
static const char key_map[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'},
};

/* ---------- 内部状态 ---------- */
static uint8_t row_now[4]    = {0};
static uint8_t row_prev[4]   = {0};
static uint8_t row_stable[4] = {0};
static volatile char key_buf = KEYPAD_NO_KEY;

typedef struct {
    PORT_Type *port;
    GPIO_Type *gpio;
    uint32_t   pin;
} pin_def_t;

static const pin_def_t row_pins[4] = {
    {ROW0_PORT, ROW0_GPIO, ROW0_PIN},
    {ROW1_PORT, ROW1_GPIO, ROW1_PIN},
    {ROW2_PORT, ROW2_GPIO, ROW2_PIN},
    {ROW3_PORT, ROW3_GPIO, ROW3_PIN},
};

static const pin_def_t col_pins[4] = {
    {COL0_PORT, COL0_GPIO, COL0_PIN},
    {COL1_PORT, COL1_GPIO, COL1_PIN},
    {COL2_PORT, COL2_GPIO, COL2_PIN},
    {COL3_PORT, COL3_GPIO, COL3_PIN},
};

/* ---------- 初始化 ---------- */
void KEYPAD_Init(void)
{
	// cometar todos los reset

    CLOCK_EnableClock(kCLOCK_GatePORT1);
    //CLOCK_EnableClock(kCLOCK_GatePORT2);
    //CLOCK_EnableClock(kCLOCK_GatePORT3);
    CLOCK_EnableClock(kCLOCK_GateGPIO1);
    //CLOCK_EnableClock(kCLOCK_GateGPIO2);
    //CLOCK_EnableClock(kCLOCK_GateGPIO3);
    /* MCXA153 必须先释放外设 reset, 否则写寄存器触发 BusFault */
	//RESET_PeripheralReset(kPORT1_RST_SHIFT_RSTn);
	//RESET_PeripheralReset(kPORT2_RST_SHIFT_RSTn);
	//RESET_PeripheralReset(kPORT3_RST_SHIFT_RSTn);
	//RESET_PeripheralReset(kGPIO1_RST_SHIFT_RSTn);
	//RESET_PeripheralReset(kGPIO2_RST_SHIFT_RSTn);
	//RESET_PeripheralReset(kGPIO3_RST_SHIFT_RSTn);

    /* 行: 推挽输出, 默认高 */
    const port_pin_config_t row_pin_cfg = {
        .pullSelect          = kPORT_PullDisable,
        .driveStrength       = kPORT_LowDriveStrength,
        .passiveFilterEnable = kPORT_PassiveFilterDisable,
        .openDrainEnable     = kPORT_OpenDrainDisable,
        .mux                 = kPORT_MuxAlt0,
        .inputBuffer         = kPORT_InputBufferEnable,
        .invertInput         = kPORT_InputNormal,
        .lockRegister        = kPORT_UnlockRegister,
    };
    const gpio_pin_config_t row_gpio_cfg = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic  = 1U,
    };

    for (int i = 0; i < 4; i++) {
        PORT_SetPinConfig(row_pins[i].port, row_pins[i].pin, &row_pin_cfg);
        GPIO_PinInit(row_pins[i].gpio, row_pins[i].pin, &row_gpio_cfg);
    }

    /* 列: 输入 + 上拉 */
    const port_pin_config_t col_pin_cfg = {
        .pullSelect          = kPORT_PullUp,
        .driveStrength       = kPORT_LowDriveStrength,
        .passiveFilterEnable = kPORT_PassiveFilterDisable,
        .openDrainEnable     = kPORT_OpenDrainDisable,
        .mux                 = kPORT_MuxAlt0,
        .inputBuffer         = kPORT_InputBufferEnable,
        .invertInput         = kPORT_InputNormal,
        .lockRegister        = kPORT_UnlockRegister,
    };
    const gpio_pin_config_t col_gpio_cfg = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic  = 0U,
    };

    for (int i = 0; i < 4; i++) {
        PORT_SetPinConfig(col_pins[i].port, col_pins[i].pin, &col_pin_cfg);
        GPIO_PinInit(col_pins[i].gpio, col_pins[i].pin, &col_gpio_cfg);
    }

    for (int i = 0; i < 4; i++) {
        row_now[i] = row_prev[i] = row_stable[i] = 0;
    }
    key_buf = KEYPAD_NO_KEY;
}

/* ---------- 扫描 ---------- */
void KEYPAD_Scan(void)
{
    for (int r = 0; r < 4; r++) {
        for (int i = 0; i < 4; i++) {
            GPIO_PinWrite(row_pins[i].gpio, row_pins[i].pin, 1U);
        }
        GPIO_PinWrite(row_pins[r].gpio, row_pins[r].pin, 0U);

        for (volatile int d = 0; d < 50; d++) { __NOP(); }

        uint8_t bits = 0;
        for (int c = 0; c < 4; c++) {
            if (GPIO_PinRead(col_pins[c].gpio, col_pins[c].pin) == 0U) {
                bits |= (1U << c);
            }
        }
        row_now[r] = bits;
    }

    for (int i = 0; i < 4; i++) {
        GPIO_PinWrite(row_pins[i].gpio, row_pins[i].pin, 1U);
    }

    for (int r = 0; r < 4; r++) {
        if (row_now[r] == row_prev[r]) {
            uint8_t newly_pressed = row_now[r] & ~row_stable[r];
            row_stable[r] = row_now[r];

            if (newly_pressed) {
                for (int c = 0; c < 4; c++) {
                    if (newly_pressed & (1U << c)) {
                        if (key_buf == KEYPAD_NO_KEY) {
                            key_buf = key_map[r][c];
                        }
                    }
                }
            }
        }
        row_prev[r] = row_now[r];
    }
}

/* ---------- 取键 ---------- */
char KEYPAD_GetKey(void)
{
    char k = key_buf;
    key_buf = KEYPAD_NO_KEY;
    return k;
}
