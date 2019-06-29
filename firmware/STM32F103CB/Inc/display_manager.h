/******************************************************************************
 * Copyright 2019 Dilshan R Jayakody. [jayakody2000lk@gmail.com]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

#ifndef DISPLAY_MANAGER_H_
#define DISPLAY_MANAGER_H_

#include "global-def.h"

#define DISPLAY_BUFFER_SIZE	MSG_SIZE

#define DISPLAY_LOW_POWER	10
#define DISPLAY_HIGH_POWER	50

#define DISPLAY_CLEAR		0x00
#define DISPLAY_DECIMAL		0x80

volatile uint8_t displayBuffer[DISPLAY_BUFFER_SIZE];

volatile uint8_t displayDelay;

unsigned char getDigitData(char value);
void setDisplayBuffer(char *content, uint8_t contentSize);
void pushCharToDisplayBuffer(char inChar);

void clearDisplay();
void updateDisplay();
void initDisplaySystem();

#endif /* DISPLAY_MANAGER_H_ */
