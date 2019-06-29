/******************************************************************************
 *
 * Part of this software component is licensed by ST under Ultimate Liberty
 * license SLA0044, the "License"; You may not use this file except in
 * compliance with the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 *****************************************************************************/

#include "main.h"
#include "usb_device.h"

#include "display_manager.h"
#include "counter_controller.h"
#include "storage_manager.h"

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

uint8_t bellCounter;
uint8_t rtcRefreshCounter;
ExtIdleMode currentIdleMode;

void initSystemClockConfig(void);
static void initRTCConfig(void);
static void initPortConfig(void);

static void initTimer2Config(void);
static void initTimer3Config(void);
static void initTimer4Config(void);

int main(void)
{
	HAL_Init();

	currentState = ECNT_IDLE_MODE;
	currentIdleMode = IDLE_TIME;
	systemMode = OP_EXTEND;
	bellState = BELL_OFF;

	lastSystemState = currentState;
	idleCounter = 0;
	bellCounter = 0;

	initSystemClockConfig();

	// Initialize system peripherals.
	initPortConfig();
	initTimer2Config();
	initTimer3Config();
	initTimer4Config();
	MX_USB_DEVICE_Init();
	initRTCConfig();

	rtcRefreshCounter = 0;

	// Initialize seven segment display system.
	initDisplaySystem();

	// Start timer 2 to update the SSD with display buffer.
	HAL_TIM_Base_Start_IT(&htim2);

	// Load saved system settings from flash memory.
	if(getBuffer() != HAL_OK)
	{
	  Error_Handler();
	}

	// At initial boot up system should be in idle state.
	updateIdleDisaplay();

	while (1)
	{
		systemMode = (HAL_GPIO_ReadPin(OP_MODE_GPIO_Port, OP_MODE_Pin) == GPIO_PIN_SET) ? OP_EXTEND : OP_DIRECT;

		if((currentState == ECNT_TIME_MODE) || (currentState == ECNT_IDLE_MODE))
		{
			currentIdleMode = getIdleModeConfig();

			if((currentState == ECNT_IDLE_MODE) && ((currentIdleMode == IDLE_CUSTOM) || (currentIdleMode == IDLE_NONE)))
			{
				HAL_Delay(100);
				continue;
			}

			// Update date and time variables in every 1 second
			if((++rtcRefreshCounter) == 10)
			{
				updateIdleDisaplay();
				rtcRefreshCounter = 0;
			}
		}

		// Check audio state and control audio out pin.
		if(bellState != BELL_OFF)
		{
			if(bellState == BELL_START)
			{
				// Activate audio buzzer.
				bellState = BELL_STOP;
				bellCounter = 0;
				HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
			}
			else if((bellState == BELL_STOP) && (bellCounter > 3))
			{
				// Deactivate audio buzzer.
				bellState = BELL_OFF;
				HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
			}

			bellCounter++;
		}

		HAL_Delay(100);
	}
}

void initSystemClockConfig(void)
{
	RCC_OscInitTypeDef sOscConfig = {0};
	RCC_ClkInitTypeDef sClockConfig = {0};
	RCC_PeriphCLKInitTypeDef sPeriphConfig = {0};

	// Initializes the CPU, AHB and APB busses clocks.

#ifdef ENABLE_EXTERNAL_RTC_CRYSTAL
	sOscConfig.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
	sOscConfig.LSEState = RCC_LSE_ON;
#else
	sOscConfig.OscillatorType = RCC_OSCILLATORTYPE_HSE;
#endif

	sOscConfig.HSEState = RCC_HSE_ON;
	sOscConfig.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	sOscConfig.HSIState = RCC_HSI_ON;
	sOscConfig.PLL.PLLState = RCC_PLL_ON;
	sOscConfig.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	sOscConfig.PLL.PLLMUL = RCC_PLL_MUL6;

	if (HAL_RCC_OscConfig(&sOscConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sClockConfig.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	sClockConfig.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	sClockConfig.AHBCLKDivider = RCC_SYSCLK_DIV1;
	sClockConfig.APB1CLKDivider = RCC_HCLK_DIV2;
	sClockConfig.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&sClockConfig, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}

	sPeriphConfig.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USB;

#ifdef ENABLE_EXTERNAL_RTC_CRYSTAL
	sPeriphConfig.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
#else
	sPeriphConfig.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV128;
#endif

	sPeriphConfig.UsbClockSelection = RCC_USBCLKSOURCE_PLL;

	if (HAL_RCCEx_PeriphCLKConfig(&sPeriphConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

static void initRTCConfig(void)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	// Initialize RTC Only.
	hrtc.Instance = RTC;
	hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
	hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;

	if (HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		Error_Handler();
	}

	// Return if RTC is already setup in previous instance.
	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) == RTC_BACKUP_FLAG)
	{
		return;
	}

	// Initialize RTC and set the Time and Date.
	sTime.Hours = 0x0;
	sTime.Minutes = 0x0;
	sTime.Seconds = 0x0;

	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
	{
		Error_Handler();
	}

	sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	sDate.Month = RTC_MONTH_JANUARY;
	sDate.Date = 0x1;
	sDate.Year = 0x0;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
	{
		Error_Handler();
	}

	// Setup flag in backup register to avoid RTC resets.
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_BACKUP_FLAG);
}

static void initTimer2Config(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 5;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 59999;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

static void initTimer3Config(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 10;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 9696;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}

	// Initialize PWM on channel 3 using timer3.
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 29000;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_TIM_MspPostInit(&htim3);
}

static void initTimer4Config(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 749;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 63999;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
	{
		Error_Handler();
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

	if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

static void initPortConfig(void)
{
	GPIO_InitTypeDef sGPIOConfig = {0};

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	// Configure GPIOA.
	HAL_GPIO_WritePin(GPIOA, SSD_A_Pin|SSD_B_Pin|SSD_C_Pin|SSD_D_Pin |SSD_E_Pin|SSD_F_Pin|SSD_G_Pin|SSD_DOT_Pin, GPIO_PIN_RESET);

	// Configure GPIOB.
	HAL_GPIO_WritePin(GPIOB, SSD_8_Pin|SSD_9_Pin|SSD_10_Pin|SSD_1_Pin |SSD_2_Pin|SSD_3_Pin|SSD_4_Pin|SSD_5_Pin |SSD_6_Pin|SSD_7_Pin, GPIO_PIN_RESET);

	sGPIOConfig.Pin = SSD_A_Pin|SSD_B_Pin|SSD_C_Pin|SSD_D_Pin |SSD_E_Pin|SSD_F_Pin|SSD_G_Pin|SSD_DOT_Pin;

	sGPIOConfig.Mode = GPIO_MODE_OUTPUT_PP;
	sGPIOConfig.Pull = GPIO_NOPULL;
	sGPIOConfig.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &sGPIOConfig);

	sGPIOConfig.Pin = OP_MODE_Pin;
	sGPIOConfig.Mode = GPIO_MODE_INPUT;
	sGPIOConfig.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(OP_MODE_GPIO_Port, &sGPIOConfig);

	sGPIOConfig.Pin = SSD_8_Pin|SSD_9_Pin|SSD_10_Pin|SSD_1_Pin |SSD_2_Pin|SSD_3_Pin|SSD_4_Pin|SSD_5_Pin |SSD_6_Pin|SSD_7_Pin;
	sGPIOConfig.Mode = GPIO_MODE_OUTPUT_PP;
	sGPIOConfig.Pull = GPIO_NOPULL;
	sGPIOConfig.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &sGPIOConfig);
}

void Error_Handler(void)
{}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
