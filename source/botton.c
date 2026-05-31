/*
 * botton.c
 *
 *  Created on: 2026年5月31日
 *      Author: dani
 */

#include "botton.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_clock.h"

static volatile bool g_button_pressed = false;

void Button_Init(void)
{
    CLOCK_EnableClock(kCLOCK_GatePORT1);
    CLOCK_EnableClock(kCLOCK_GateGPIO1);

    // 引脚配置：GPIO 模式 + 上拉 + 输入使能
    port_pin_config_t pin_cfg = {
        .pullSelect          = kPORT_PullUp,
        .driveStrength       = kPORT_LowDriveStrength,
        .passiveFilterEnable = kPORT_PassiveFilterDisable,
        .openDrainEnable     = kPORT_OpenDrainDisable,
        .mux                 = kPORT_MuxAlt0,
        .inputBuffer         = kPORT_InputBufferEnable,
        .invertInput         = kPORT_InputNormal,
        .lockRegister        = kPORT_UnlockRegister,
    };
    PORT_SetPinConfig(HOME_BTN_PORT, HOME_BTN_PIN, &pin_cfg);

    gpio_pin_config_t gpio_cfg = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic  = 0U,
    };
    GPIO_PinInit(HOME_BTN_GPIO, HOME_BTN_PIN, &gpio_cfg);

    // MCXA153 用 GPIO 模块配置中断，不是 PORT 模块
    GPIO_SetPinInterruptConfig(HOME_BTN_GPIO, HOME_BTN_PIN, kGPIO_InterruptFallingEdge);

    // 清挂起 + 使能 NVIC
    GPIO_PortClearInterruptFlags(HOME_BTN_GPIO, 1U << HOME_BTN_PIN);
    EnableIRQ(HOME_BTN_IRQn);
}

bool Button_Pressed(void)
{
    if (g_button_pressed) {
        g_button_pressed = false;
        return true;
    }
    return false;
}

// 中断处理
void GPIO1_IRQHandler(void)
{
    uint32_t flags = GPIO_PortGetInterruptFlags(HOME_BTN_GPIO);
    if (flags & (1U << HOME_BTN_PIN))
    {
        GPIO_PortClearInterruptFlags(HOME_BTN_GPIO, 1U << HOME_BTN_PIN);
        g_button_pressed = true;
    }
}
