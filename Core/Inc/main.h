/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_PIN_Pin GPIO_PIN_13
#define LED_PIN_GPIO_Port GPIOC
#define USER_SW2_Pin GPIO_PIN_2
#define USER_SW2_GPIO_Port GPIOA
#define USER_SW2_EXTI_IRQn EXTI2_IRQn
#define FRONT_SW1_PIN_Pin GPIO_PIN_3
#define FRONT_SW1_PIN_GPIO_Port GPIOA
#define FRONT_SW1_PIN_EXTI_IRQn EXTI3_IRQn
#define FRONT_SW2_PIN_Pin GPIO_PIN_4
#define FRONT_SW2_PIN_GPIO_Port GPIOA
#define FRONT_SW2_PIN_EXTI_IRQn EXTI4_IRQn
#define FRONT_SW3_PIN_Pin GPIO_PIN_5
#define FRONT_SW3_PIN_GPIO_Port GPIOA
#define FRONT_SW3_PIN_EXTI_IRQn EXTI9_5_IRQn
#define FRONT_SW4_PIN_Pin GPIO_PIN_6
#define FRONT_SW4_PIN_GPIO_Port GPIOA
#define FRONT_SW4_PIN_EXTI_IRQn EXTI9_5_IRQn
#define FRONT_SW5_PIN_Pin GPIO_PIN_7
#define FRONT_SW5_PIN_GPIO_Port GPIOA
#define FRONT_SW5_PIN_EXTI_IRQn EXTI9_5_IRQn
#define OUT_PIN1_Pin GPIO_PIN_0
#define OUT_PIN1_GPIO_Port GPIOB
#define OUT_PIN2_Pin GPIO_PIN_1
#define OUT_PIN2_GPIO_Port GPIOB
#define OUT_PIN6_Pin GPIO_PIN_12
#define OUT_PIN6_GPIO_Port GPIOB
#define MAX7219_CS_Pin GPIO_PIN_14
#define MAX7219_CS_GPIO_Port GPIOB
#define OUT_PIN3_Pin GPIO_PIN_8
#define OUT_PIN3_GPIO_Port GPIOA
#define USER_SW3_Pin GPIO_PIN_15
#define USER_SW3_GPIO_Port GPIOA
#define OUT_PIN4_Pin GPIO_PIN_8
#define OUT_PIN4_GPIO_Port GPIOB
#define OUT_PIN5_Pin GPIO_PIN_9
#define OUT_PIN5_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
