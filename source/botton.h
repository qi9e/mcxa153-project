/*
 * botton.h
 *
 *  Created on: 2026年5月31日
 *      Author: dani
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

// FRDM-MCXA153 板载 SW2 = P1_7
#define HOME_BTN_PORT   PORT1
#define HOME_BTN_GPIO   GPIO1
#define HOME_BTN_PIN    7U
#define HOME_BTN_IRQn   GPIO1_IRQn

void Button_Init(void);
bool Button_Pressed(void);

#endif

