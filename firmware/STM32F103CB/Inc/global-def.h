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

#ifndef GLOBAL_DEF_H_
#define GLOBAL_DEF_H_

#include <stdint.h>

#include "stm32f1xx_hal.h"

typedef enum
{
	ECNT_MSG_MODE,
	ECNT_TIME_MODE,		// Reserved for extensibility.
	ECNT_DATE_MODE,		// Reserved for extensibility.
	ECNT_IDLE_MODE
} ExtCounterMode;

typedef enum
{
	EOP_IDLE,
	EOP_NORMAL
} ExtCounterIdle;

typedef enum
{
	IDLE_NONE = 0,
	IDLE_TIME = 1,
	IDLE_DATE = 2,
	IDLE_CUSTOM = 3
} ExtIdleMode;

typedef enum
{
	OP_DIRECT,
	OP_EXTEND
} ExtOpMode;

typedef enum
{
	BELL_OFF,
	BELL_START,
	BELL_STOP
} ExtAudioState;

#define MSG_SIZE 10

TIM_HandleTypeDef htim4;
RTC_HandleTypeDef hrtc;

ExtCounterMode currentState;
ExtCounterMode lastSystemState;

ExtOpMode systemMode;
ExtAudioState bellState;

uint16_t idleCounter;

#endif /* GLOBAL_DEF_H_ */
