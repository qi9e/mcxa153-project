/*
 * keypad.c
 *
 *  Created on: 2026年4月31日
 *      Author: dani
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdint.h>

#define KEYPAD_NO_KEY   ((char)0)

void KEYPAD_Init(void);
void KEYPAD_Scan(void);
char KEYPAD_GetKey(void);

#endif /* KEYPAD_H_ */
