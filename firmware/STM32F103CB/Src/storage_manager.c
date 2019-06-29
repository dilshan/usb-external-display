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

#include <string.h>

#include "storage_manager.h"

static HAL_StatusTypeDef flashBuffer(char *msgBuffer, uint8_t contentSize)
{
	HAL_StatusTypeDef retValue;
	FLASH_EraseInitTypeDef eraseConfig = {0};
	uint32_t pageErr = 0;
	uint32_t memAddr;
	uint8_t contentPos = 0;

	if(contentSize > 0)
	{
		retValue = HAL_FLASH_Unlock();
		if(retValue != HAL_OK)
		{
			return retValue;
		}

		// Erase flash memory page.
		eraseConfig.TypeErase = FLASH_TYPEERASE_PAGES;
		eraseConfig.PageAddress = FLASH_STORAGE_START_ADDR;
		eraseConfig.NbPages = (FLASH_STORAGE_END_ADDR - FLASH_STORAGE_START_ADDR) / FLASH_PAGE_SIZE;
		retValue = HAL_FLASHEx_Erase(&eraseConfig, &pageErr);
		if(retValue != HAL_OK)
		{
			return retValue;
		}

		// Write data buffer into flash memory page.
		memAddr = FLASH_STORAGE_START_ADDR;
		for(contentPos = 0; contentPos < contentSize; contentPos++)
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, memAddr, msgBuffer[contentPos]);
			memAddr += 2;
		}

		// Release flash memory.
		retValue = HAL_FLASH_Lock();
		if(retValue != HAL_OK)
		{
			return retValue;
		}

		return HAL_OK;
	}

	return HAL_ERROR;
}

static HAL_StatusTypeDef readBuffer(char *outBuffer, uint8_t contentSize)
{
	uint8_t contentPos = 0;
	uint32_t memAddr = FLASH_STORAGE_START_ADDR;
	uint32_t tempData;

	for(contentPos = 0; contentPos < contentSize; contentPos++)
	{
		tempData =  *(__IO uint32_t *)(memAddr);
		outBuffer[contentPos] = (char)(tempData);
		memAddr += 2;
	}

	return HAL_OK;
}

HAL_StatusTypeDef setBuffer()
{
	return flashBuffer(storageBuffer, STORAGE_BUFFER_SIZE);
}

HAL_StatusTypeDef getBuffer()
{
	return readBuffer(storageBuffer, STORAGE_BUFFER_SIZE);
}

uint8_t setCustomMsg(char *msgBuffer)
{
	memcpy(storageBuffer, msgBuffer, MSG_SIZE);
	setBuffer();

	return MSG_SIZE;
}

uint8_t getCustomMsg(char *msgBuffer)
{
	memcpy(msgBuffer, storageBuffer, MSG_SIZE);

	return MSG_SIZE;
}

void setIdleModeConfig(ExtIdleMode modeData)
{
	storageBuffer[MSG_SIZE + 2] = (char)modeData;
	setBuffer();
}

ExtIdleMode getIdleModeConfig()
{
	char idleConfog = storageBuffer[MSG_SIZE + 2];
	if(idleConfog == 0xFF)
	{
		return IDLE_TIME;
	}

	return idleConfog;
}
