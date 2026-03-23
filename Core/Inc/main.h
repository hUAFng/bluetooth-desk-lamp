/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* ----------------------------ASRPRO----------------------------- */
#define ASR_PA1_Pin GPIO_PIN_1
#define ASR_PA1_GPIO_Port GPIOA
#define ASR_PA4_Pin GPIO_PIN_4
#define ASR_PA4_GPIO_Port GPIOA
#define ASR_TX_Pin GPIO_PIN_9
#define ASR_TX_GPIO_Port GPIOA
#define ASR_RX_Pin GPIO_PIN_10
#define ASR_RX_GPIO_Port GPIOA
#define ASR_PB9_Pin GPIO_PIN_9
#define ASR_PB9_GPIO_Port GPIOB

/* ----------------------------bluetooth----------------------------- */
#define BT_TX_Pin GPIO_PIN_2
#define BT_TX_GPIO_Port GPIOA
#define BT_RX_Pin GPIO_PIN_3
#define BT_RX_GPIO_Port GPIOA

/* ---------------------------------KEY ---------------------------- */
#define Key1_Pin GPIO_PIN_0
#define Key1_GPIO_Port GPIOB
#define Key1_EXTI_IRQn EXTI0_IRQn
#define Key2_Pin GPIO_PIN_1
#define Key2_GPIO_Port GPIOB
#define Key2_EXTI_IRQn EXTI1_IRQn
#define Key3_Pin GPIO_PIN_3
#define Key3_GPIO_Port GPIOB
#define Key3_EXTI_IRQn EXTI3_IRQn

/* ---------------------------------TFT ---------------------------- */
#define TFT_RES_Pin GPIO_PIN_10
#define TFT_RES_GPIO_Port GPIOB
#define TFT_DC_Pin GPIO_PIN_11
#define TFT_DC_GPIO_Port GPIOB
#define TFT_CS_Pin GPIO_PIN_12
#define TFT_CS_GPIO_Port GPIOB
#define TFT_BL_Pin GPIO_PIN_8
#define TFT_BL_GPIO_Port GPIOB

/* ---------------------------------RGB ---------------------------- */
#define RGB_Pin GPIO_PIN_8
#define RGB_GPIO_Port GPIOA

/* ---------------------------------Buzzer ---------------------------- */
#define Buzzer_Pin GPIO_PIN_5
#define Buzzer_GPIO_Port GPIOB

/* ---------------------------------LightSensor ---------------------------- */
#define LightSensor_SCL_Pin GPIO_PIN_6
#define LightSensor_SCL_GPIO_Port GPIOB
#define LightSensor_SDK_Pin GPIO_PIN_7
#define LightSensor_SDK_GPIO_Port GPIOB


/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
