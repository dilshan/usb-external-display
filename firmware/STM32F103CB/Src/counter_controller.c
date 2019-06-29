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

#include "counter_controller.h"
#include "display_manager.h"
#include "storage_manager.h"
#include "global-def.h"

HAL_StatusTypeDef processText(char* Buf, uint32_t* Len)
{
	uint32_t dataLen = *Len;
	int32_t dataPos = (dataLen - 1);

	if(dataLen > 0)
	{
		if(currentState == ECNT_IDLE_MODE)
		{
			setSystemIdleState(EOP_NORMAL);
			currentState = ECNT_MSG_MODE;

			clearDisplay();
		}

		while(dataPos >= 0)
		{
			pushCharToDisplayBuffer(Buf[dataPos]);
			dataPos--;
		}
	}

	return HAL_OK;
}

HAL_StatusTypeDef processCommand(char* Buf, uint32_t* Len)
{
	// Check for valid command length and header.
	uint32_t dataLen = *Len;

	if((dataLen > 1) && (Buf[0] == 94))
	{
		if(Buf[1] == '0')
		{
			// Clear display unit.
			setSystemIdleState(EOP_NORMAL);

			currentState = ECNT_MSG_MODE;

			clearDisplay();
			return HAL_OK;
		}
		else if((Buf[1] == '1') && (dataLen > 2))
		{
			// Display content on the screen.
			uint32_t dataBufferLen = dataLen - 2;
			char tempData[dataBufferLen];

			setSystemIdleState(EOP_NORMAL);

			currentState = ECNT_MSG_MODE;

			memcpy(tempData, (Buf + 2), dataBufferLen);
			setDisplayBuffer(tempData, dataBufferLen);

			return HAL_OK;
		}
		else if((Buf[1] == '2') && (dataLen == 5))
		{
			// Set time of the unit.
			RTC_TimeTypeDef sTime = {0};

			if((Buf[2] < 0) || (Buf[3] < 0) || (Buf[4] < 0))
			{
				return HAL_ERROR;
			}

			if((Buf[2] > 23) || (Buf[3] > 59) || (Buf[4] > 59))
			{
				return HAL_ERROR;
			}

			sTime.Hours = Buf[2];
			sTime.Minutes = Buf[3];
			sTime.Seconds = Buf[4];

			return HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		}

#ifdef ENABLE_RTC_DATE
		else if((Buf[1] == '3') && (dataLen == 5))
		{
			// Set date of the unit.
			RTC_DateTypeDef sDate = {0};

			if((Buf[2] < 1) || (Buf[3] < 1) || (Buf[4] < 1))
			{
				return HAL_ERROR;
			}

			if((Buf[2] > 31) || (Buf[3] > 12) || (Buf[4] > 99))
			{
				return HAL_ERROR;
			}

			sDate.Date = Buf[2];
			sDate.Month = Buf[3];
			sDate.Year = Buf[4];

			return HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		}
#endif

		else if(Buf[1] == '4')
		{
			// Switch device into idle mode.
			setSystemIdleState(EOP_IDLE);
			return HAL_OK;
		}
		else if((Buf[1] == '5') && (dataLen > 2))
		{
			// Set custom idle message.
			uint32_t dataBufferLen = dataLen - 2;
			char tempData[MSG_SIZE];

			memset(tempData, DISPLAY_CLEAR, MSG_SIZE);

			memcpy(tempData, (Buf + 2), dataBufferLen);
			setCustomMsg(tempData);

			return HAL_OK;
		}
		else if((Buf[1] == '6') && (dataLen == 3))
		{
			// Set idle mode configuration.
			if(Buf[2] < 4)
			{
				setIdleModeConfig(Buf[2]);
				if(currentState == ECNT_IDLE_MODE)
				{
					updateIdleDisaplay();
				}
				return HAL_OK;
			}
		}
		else if(Buf[1] == '7')
		{
			// Trigger system bell.
			if(currentState != ECNT_IDLE_MODE)
			{
				bellState = BELL_START;
			}
		}
	}

	return HAL_ERROR;
}

void showSystemTime()
{
	const char *outputTemplate;
	char dateOutput[DISPLAY_BUFFER_SIZE];
	RTC_TimeTypeDef sTime = {0};

	memset(dateOutput, (int)NULL, DISPLAY_BUFFER_SIZE);

	if(HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) == HAL_OK)
	{
		outputTemplate = ((sTime.Seconds % 2) == 0) ? "   %02d.%02d   " : "   %02d%02d   ";
		sprintf(dateOutput, outputTemplate, sTime.Hours, sTime.Minutes);
		setDisplayBuffer(dateOutput, DISPLAY_BUFFER_SIZE);
	}
}

#ifdef ENABLE_RTC_DATE
void showSystemDate()
{
	char dateOutput[DISPLAY_BUFFER_SIZE];
	RTC_DateTypeDef sDate = {0};

	memset(dateOutput, (int)NULL, DISPLAY_BUFFER_SIZE);

	if(HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) == HAL_OK)
	{
		sprintf(dateOutput, "%02d %02d %04d", sDate.Date, sDate.Month, (sDate.Year + 2000));
		setDisplayBuffer(dateOutput, DISPLAY_BUFFER_SIZE);
	}
}
#endif

void showCustomIdleMsg()
{
	char msgBuffer[DISPLAY_BUFFER_SIZE];

	getCustomMsg(msgBuffer);
	setDisplayBuffer(msgBuffer, DISPLAY_BUFFER_SIZE);
}

void setSystemIdleState(ExtCounterIdle idleState)
{
	if(idleState == EOP_IDLE)
	{
		if(currentState != ECNT_IDLE_MODE)
		{
			// System is switching to idle mode.
			HAL_TIM_Base_Stop_IT(&htim4);

			lastSystemState = currentState;
			currentState = ECNT_IDLE_MODE;

			displayDelay = DISPLAY_LOW_POWER;
			updateIdleDisaplay();
		}
	}
	else if(currentState == ECNT_IDLE_MODE)
	{
		// System is recovering from idle mode.
		currentState = lastSystemState;

		displayDelay = DISPLAY_HIGH_POWER;

		idleCounter = 0;
		HAL_TIM_Base_Start_IT(&htim4);
	}
}

void updateIdleDisaplay()
{
	switch(getIdleModeConfig())
	{
		case IDLE_CUSTOM:
			// Show user defined message in idle mode.
			showCustomIdleMsg();
			break;
		case IDLE_TIME:
			// Show system time in idle mode.
			showSystemTime();
			break;
#ifdef ENABLE_RTC_DATE
		case IDLE_DATE:
			// Show system date in idle mode.
			showSystemDate();
			break;
#endif
		case IDLE_NONE:
			// Clear display in idle mode.
			clearDisplay();
			break;
		default:
			showSystemTime();
	}
}
