/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#define RIGHT_Pin GPIO_PIN_13
#define RIGHT_GPIO_Port GPIOC
#define TRIG_Pin GPIO_PIN_0
#define TRIG_GPIO_Port GPIOC
#define ECHO_Pin GPIO_PIN_1
#define ECHO_GPIO_Port GPIOC
#define ECHO_EXTI_IRQn EXTI1_IRQn
#define LEFT_MOTOR_GO_Pin GPIO_PIN_12
#define LEFT_MOTOR_GO_GPIO_Port GPIOB
#define LEFT2_MOTOR_GO_Pin GPIO_PIN_13
#define LEFT2_MOTOR_GO_GPIO_Port GPIOB
#define RIGHT_MOTOR_GO_Pin GPIO_PIN_14
#define RIGHT_MOTOR_GO_GPIO_Port GPIOB
#define RIGHT2_MOTOR_GO_Pin GPIO_PIN_15
#define RIGHT2_MOTOR_GO_GPIO_Port GPIOB
#define LEFT_Pin GPIO_PIN_11
#define LEFT_GPIO_Port GPIOC
#define MID_Pin GPIO_PIN_12
#define MID_GPIO_Port GPIOC
#define L2_Pin GPIO_PIN_2
#define L2_GPIO_Port GPIOD
#define LEFT_MOTOR_PWM_Pin GPIO_PIN_6
#define LEFT_MOTOR_PWM_GPIO_Port GPIOB
#define LEFT2_MOTOR_PWM_Pin GPIO_PIN_7
#define LEFT2_MOTOR_PWM_GPIO_Port GPIOB
#define RIGHT_MOTOR_PWM_Pin GPIO_PIN_8
#define RIGHT_MOTOR_PWM_GPIO_Port GPIOB
#define RIGHT2_MOTOR_PWM_Pin GPIO_PIN_9
#define RIGHT2_MOTOR_PWM_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
