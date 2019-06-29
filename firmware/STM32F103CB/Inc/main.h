/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#define RTC_BACKUP_FLAG 102

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SSD_A_Pin GPIO_PIN_0
#define SSD_A_GPIO_Port GPIOA
#define SSD_B_Pin GPIO_PIN_1
#define SSD_B_GPIO_Port GPIOA
#define SSD_C_Pin GPIO_PIN_2
#define SSD_C_GPIO_Port GPIOA
#define SSD_D_Pin GPIO_PIN_3
#define SSD_D_GPIO_Port GPIOA
#define SSD_E_Pin GPIO_PIN_4
#define SSD_E_GPIO_Port GPIOA
#define SSD_F_Pin GPIO_PIN_5
#define SSD_F_GPIO_Port GPIOA
#define SSD_G_Pin GPIO_PIN_6
#define SSD_G_GPIO_Port GPIOA
#define SSD_DOT_Pin GPIO_PIN_7
#define SSD_DOT_GPIO_Port GPIOA
#define SPK_OUT_Pin GPIO_PIN_0
#define SPK_OUT_GPIO_Port GPIOB
#define OP_MODE_Pin GPIO_PIN_1
#define OP_MODE_GPIO_Port GPIOB
#define SSD_8_Pin GPIO_PIN_10
#define SSD_8_GPIO_Port GPIOB
#define SSD_9_Pin GPIO_PIN_11
#define SSD_9_GPIO_Port GPIOB
#define SSD_10_Pin GPIO_PIN_12
#define SSD_10_GPIO_Port GPIOB
#define SSD_1_Pin GPIO_PIN_3
#define SSD_1_GPIO_Port GPIOB
#define SSD_2_Pin GPIO_PIN_4
#define SSD_2_GPIO_Port GPIOB
#define SSD_3_Pin GPIO_PIN_5
#define SSD_3_GPIO_Port GPIOB
#define SSD_4_Pin GPIO_PIN_6
#define SSD_4_GPIO_Port GPIOB
#define SSD_5_Pin GPIO_PIN_7
#define SSD_5_GPIO_Port GPIOB
#define SSD_6_Pin GPIO_PIN_8
#define SSD_6_GPIO_Port GPIOB
#define SSD_7_Pin GPIO_PIN_9
#define SSD_7_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
