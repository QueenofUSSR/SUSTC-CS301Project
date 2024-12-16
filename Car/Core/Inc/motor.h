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
#ifndef __MOTOR_H
#define __MOTOR_H

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
#define K3_Pin GPIO_PIN_0
#define K3_GPIO_Port GPIOA
#define K3_EXTI_IRQn EXTI0_IRQn
#define K1_Pin GPIO_PIN_4
#define K1_GPIO_Port GPIOC
#define K1_EXTI_IRQn EXTI4_IRQn
#define K2_Pin GPIO_PIN_5
#define K2_GPIO_Port GPIOC
#define K2_EXTI_IRQn EXTI9_5_IRQn
#define L1_Pin GPIO_PIN_0
#define L1_GPIO_Port GPIOB
#define LEFT_MOTOR_GO_Pin GPIO_PIN_12
#define LEFT_MOTOR_GO_GPIO_Port GPIOB
#define LEFT2_MOTOR_GO_Pin GPIO_PIN_13
#define LEFT2_MOTOR_GO_GPIO_Port GPIOB
#define RIGHT_MOTOR_GO_Pin GPIO_PIN_14
#define RIGHT_MOTOR_GO_GPIO_Port GPIOB
#define RIGHT2_MOTOR_GO_Pin GPIO_PIN_15
#define RIGHT2_MOTOR_GO_GPIO_Port GPIOB
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
void motorInit();
void setMotorSpeed(TIM_HandleTypeDef *htim, uint32_t Channel, int16_t speed);
//void motorRunTest(TIM_HandleTypeDef *htim);
//void motorBackTest(TIM_HandleTypeDef *htim);
//void motorRun(int16_t speed);
void motorBreak();
//void motorBack(int16_t speed);
//void motorTurnLeft(int16_t speed);
//void motorSpinLeft(int16_t speed);
//void motorTurnRight(int16_t speed);
//void motorSpinRight(int16_t speed);
//void motorRunDelay(int16_t speed, int time);
//void motorBreakDelay(int time);
//void motorBackDelay(int16_t speed, int time);
//void motorTurnLeftDelay(int16_t speed, int time);
//void motorSpinLeftDelay(int16_t speed, int time);
//void motorTurnRightDelay(int16_t speed, int time);
//void motorSpinRightDelay(int16_t speed, int time);
void moveForward();
void moveBackward();
void moveLeft();
void moveRight();
void moveTurnLeft();
void moveTurnRight();
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
