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

#ifndef STORAGE_MANAGER_H_
#define STORAGE_MANAGER_H_

#include <stdint.h>

#include "stm32f1xx_hal.h"
#include "global-def.h"

#define FLASH_STORAGE_START_ADDR	((uint32_t)0x0800C000)
#define FLASH_STORAGE_END_ADDR		FLASH_STORAGE_START_ADDR + FLASH_PAGE_SIZE

#define STORAGE_BUFFER_SIZE	16

char storageBuffer[STORAGE_BUFFER_SIZE];

HAL_StatusTypeDef setBuffer();
HAL_StatusTypeDef getBuffer();

HAL_StatusTypeDef setCustomMsg(char *msgBuffer);
uint8_t getCustomMsg(char *msgBuffer);

void setIdleModeConfig(ExtIdleMode modeData);
ExtIdleMode getIdleModeConfig();

#endif /* STORAGE_MANAGER_H_ */
