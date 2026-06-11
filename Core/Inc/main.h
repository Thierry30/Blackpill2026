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
#include "stm32f4xx_hal.h"

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
#define LedBleu_Carte_Pin GPIO_PIN_13
#define LedBleu_Carte_GPIO_Port GPIOC
#define EncoG_Pin GPIO_PIN_0
#define EncoG_GPIO_Port GPIOA
#define TX_Bt_Pin GPIO_PIN_2
#define TX_Bt_GPIO_Port GPIOA
#define RX_Bt_Pin GPIO_PIN_3
#define RX_Bt_GPIO_Port GPIOA
#define Cpt_Stop_Pin GPIO_PIN_10
#define Cpt_Stop_GPIO_Port GPIOB
#define CS_Mem_Pin GPIO_PIN_12
#define CS_Mem_GPIO_Port GPIOB
#define AIN1_MotG_Pin GPIO_PIN_14
#define AIN1_MotG_GPIO_Port GPIOB
#define BIN1_MotD_Pin GPIO_PIN_15
#define BIN1_MotD_GPIO_Port GPIOB
#define PWM_Mot_G_Pin GPIO_PIN_8
#define PWM_Mot_G_GPIO_Port GPIOA
#define PWM_Mot_D_Pin GPIO_PIN_9
#define PWM_Mot_D_GPIO_Port GPIOA
#define CS_Mcp3008_Pin GPIO_PIN_15
#define CS_Mcp3008_GPIO_Port GPIOA
#define EncoD_Pin GPIO_PIN_4
#define EncoD_GPIO_Port GPIOB
#define AIN2_MotG_Pin GPIO_PIN_5
#define AIN2_MotG_GPIO_Port GPIOB
#define BIN2_MotD_Pin GPIO_PIN_6
#define BIN2_MotD_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
