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
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define LoRa_NRST_Pin GPIO_PIN_0
#define LoRa_NRST_GPIO_Port GPIOC
#define LoRa_DIO1_Pin GPIO_PIN_1
#define LoRa_DIO1_GPIO_Port GPIOC
#define LoRa_DIO1_EXTI_IRQn EXTI1_IRQn
#define LoRa_Busy_Pin GPIO_PIN_2
#define LoRa_Busy_GPIO_Port GPIOC
#define LoRa_RF_SW_Pin GPIO_PIN_3
#define LoRa_RF_SW_GPIO_Port GPIOC
#define SPI_NSS_LoRa_Pin GPIO_PIN_4
#define SPI_NSS_LoRa_GPIO_Port GPIOA
#define SPI_SCK_LoRa_Pin GPIO_PIN_5
#define SPI_SCK_LoRa_GPIO_Port GPIOA
#define SPI1_MISO_LoRa_Pin GPIO_PIN_6
#define SPI1_MISO_LoRa_GPIO_Port GPIOA
#define SPI1_MOSI_LoRa_Pin GPIO_PIN_7
#define SPI1_MOSI_LoRa_GPIO_Port GPIOA
#define WP_FLASH_Pin GPIO_PIN_1
#define WP_FLASH_GPIO_Port GPIOB
#define HOLD_FLASH_Pin GPIO_PIN_2
#define HOLD_FLASH_GPIO_Port GPIOB
#define SPI2_NSS_FLASH_Pin GPIO_PIN_12
#define SPI2_NSS_FLASH_GPIO_Port GPIOB
#define SPI2_SCK_FLASH_Pin GPIO_PIN_13
#define SPI2_SCK_FLASH_GPIO_Port GPIOB
#define SPI2_MISO_FLASH_Pin GPIO_PIN_14
#define SPI2_MISO_FLASH_GPIO_Port GPIOB
#define SPI2_MOSI_FLASH_Pin GPIO_PIN_15
#define SPI2_MOSI_FLASH_GPIO_Port GPIOB
#define SD_INSERT_GPIO_Pin GPIO_PIN_7
#define SD_INSERT_GPIO_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
