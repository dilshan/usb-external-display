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

#include "display_manager.h"

unsigned char getDigitData(char value)
{
	switch(value)
	{
		case 48:			// 0
			return 0x3F;
		case 49:			// 1
			return 0x06;
		case 50:			// 2
			return 0x5B;
		case 51:			// 3
			return 0x4F;
		case 52:			// 4
			return 0x66;
		case 53:			// 5
			return 0x6D;
		case 54:			// 6
			return 0x7D;
		case 55:			// 7
			return 0x07;
		case 56:			// 8
			return 0x7F;
		case 57:			// 9
			return 0x6F;
		case 65:			// A
		case 97:
			return 0x77;
		case 66:			// B
		case 98:
			return 0x7C;
		case 67:			// C
		case 99:
			return 0x39;
		case 68:			// D
		case 100:
			return 0x5E;
		case 69:			// E
		case 101:
			return 0x79;
		case 70:			// F
		case 102:
			return 0x71;
		case 45:			// Minus symbol
			return 0x40;
		default:			// Clear
			return 0;
	}
}

void pushCharToDisplayBuffer(char inChar)
{
	uint8_t bufferPos = DISPLAY_BUFFER_SIZE - 1;

	// Handle special characters received from host.
	if(inChar == 7)
	{
		bellState = BELL_START;
	}
	else if((inChar == 13) || (inChar == 10))
	{
		clearDisplay();
	}
	else
	{
		// Fill characters received from host into display buffer.
		while(bufferPos > 0)
		{
			displayBuffer[bufferPos] = displayBuffer[bufferPos - 1];
			bufferPos--;
		}

		displayBuffer[0] = getDigitData(inChar);
		updateDisplay();
	}
}

void setDisplayBuffer(char *content, uint8_t contentSize)
{
	uint8_t bufferPos = 0;
	uint8_t contentPos = 0;

	if(contentSize > 0)
	{
		while(bufferPos < DISPLAY_BUFFER_SIZE)
		{
			if(content[contentPos] == '.')
			{
				// Add decimal symbol to the previous segment info.
				if(bufferPos > 0)
				{
					displayBuffer[bufferPos - 1] |= DISPLAY_DECIMAL;
				}
			}
			else
			{
				displayBuffer[bufferPos] = (bufferPos <= contentSize) ? getDigitData(content[contentPos]) : DISPLAY_CLEAR;
				bufferPos++;
			}

			if(contentPos < contentSize)
			{
				contentPos++;
			}
		}

		// Display buffer update is finished. Lets push this new display buffer to SSD.
		updateDisplay();
	}
}

void clearDisplay()
{
	memset((uint8_t*)displayBuffer, DISPLAY_CLEAR, DISPLAY_BUFFER_SIZE);
	updateDisplay();
}

void initDisplaySystem()
{
	displayDelay = DISPLAY_LOW_POWER;

	memset((uint8_t*)displayBuffer, DISPLAY_CLEAR, DISPLAY_BUFFER_SIZE);
	GPIOA->ODR &= 0xFF00;

	if (!((CoreDebug->DEMCR) & (CoreDebug_DEMCR_TRCENA_Msk)))
	{
			CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	        DWT->CYCCNT = 0;
	        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}
}

void updateDisplay()
{
	uint8_t displayPos = 0;
	uint32_t startTick, endTick;
	uint16_t segmentPos;

	GPIOA->ODR &= 0xFF00;

	while(displayPos < DISPLAY_BUFFER_SIZE)
	{
		segmentPos = GPIO_PIN_3 << displayPos;

		// Update segment with display buffer content.
		GPIOB->BSRR = segmentPos;
		GPIOA->ODR |= displayBuffer[displayPos];

		// Wait for few micro-seconds (defined in SEGMENT_DELAY).
		startTick = DWT->CYCCNT;
		endTick = (SystemCoreClock/1000000) * displayDelay;
		while ((DWT->CYCCNT - startTick) < endTick);

		// Shutdown current segment and move to next segment.
		GPIOB->BSRR = (uint32_t)segmentPos << 16U;
		GPIOA->ODR &= 0xFF00;
		displayPos++;

		startTick = DWT->CYCCNT;
		endTick = (SystemCoreClock/1000000) * 5;
		while ((DWT->CYCCNT - startTick) < endTick);
	}
}
