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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
// #include <cerrno>
#include <errno.h>
#include <BNO055-1f722ffec323/BNO055.h>
#include <DitelMotorDriverController/DitelMotorDriverController.h>
#include <TripleBufferSystem/TripleBufferSystem.hpp>
#include <VelPid/VelPid.hpp>
#include <Encoder/Encoder.hpp>

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
#define DebugButton_Pin GPIO_PIN_13
#define DebugButton_GPIO_Port GPIOC
#define Encoder2_1_Pin GPIO_PIN_0
#define Encoder2_1_GPIO_Port GPIOA
#define Encoder2_2_Pin GPIO_PIN_1
#define Encoder2_2_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define DebugLED_Pin GPIO_PIN_5
#define DebugLED_GPIO_Port GPIOA
#define Encoder3_1_Pin GPIO_PIN_6
#define Encoder3_1_GPIO_Port GPIOA
#define Encoder3_2_Pin GPIO_PIN_7
#define Encoder3_2_GPIO_Port GPIOA
#define Encoder1_1_Pin GPIO_PIN_8
#define Encoder1_1_GPIO_Port GPIOA
#define Encoder1_2_Pin GPIO_PIN_9
#define Encoder1_2_GPIO_Port GPIOA
#define CAN1_RX_motorDriver_Pin GPIO_PIN_11
#define CAN1_RX_motorDriver_GPIO_Port GPIOA
#define CAN1_TX_motorDriver_Pin GPIO_PIN_12
#define CAN1_TX_motorDriver_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define USART3_TX_ESP_Pin GPIO_PIN_10
#define USART3_TX_ESP_GPIO_Port GPIOC
#define USART3_RX_ESP_Pin GPIO_PIN_11
#define USART3_RX_ESP_GPIO_Port GPIOC
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define Encoder4_1_Pin GPIO_PIN_6
#define Encoder4_1_GPIO_Port GPIOB
#define Encoder4_2_Pin GPIO_PIN_7
#define Encoder4_2_GPIO_Port GPIOB
#define I2C1_SCL_BNO_Pin GPIO_PIN_8
#define I2C1_SCL_BNO_GPIO_Port GPIOB
#define I2C1_SDA_BNO_Pin GPIO_PIN_9
#define I2C1_SDA_BNO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
