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
#include "stm32l4xx_hal.h"

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
#define NXP_UART_PORT_NUM 3
#define LTE_UART_PORT_NUM 5
#define GPS_UART_PORT_NUM 4
#define PRT_UART_PORT_NUM 2
#define LTE_DTR_Pin GPIO_PIN_2
#define LTE_DTR_GPIO_Port GPIOE
#define LTE_RESET_Pin GPIO_PIN_3
#define LTE_RESET_GPIO_Port GPIOE
#define LTE_ON_SLEEP_Pin GPIO_PIN_4
#define LTE_ON_SLEEP_GPIO_Port GPIOE
#define GPS_RESET_Pin GPIO_PIN_5
#define GPS_RESET_GPIO_Port GPIOE
#define GPS_WAKE_Pin GPIO_PIN_6
#define GPS_WAKE_GPIO_Port GPIOE
#define TH_Pin GPIO_PIN_0
#define TH_GPIO_Port GPIOA
#define nLED_Pin GPIO_PIN_1
#define nLED_GPIO_Port GPIOA
#define FLASH_CS_Pin GPIO_PIN_2
#define FLASH_CS_GPIO_Port GPIOA
#define FLASH_CLK_Pin GPIO_PIN_3
#define FLASH_CLK_GPIO_Port GPIOA
#define FLASH_IO3_Pin GPIO_PIN_6
#define FLASH_IO3_GPIO_Port GPIOA
#define FLASH_IO2_Pin GPIO_PIN_7
#define FLASH_IO2_GPIO_Port GPIOA
#define NXP_TX_Pin GPIO_PIN_4
#define NXP_TX_GPIO_Port GPIOC
#define NXP_RX_Pin GPIO_PIN_5
#define NXP_RX_GPIO_Port GPIOC
#define FLASH_IO1_Pin GPIO_PIN_0
#define FLASH_IO1_GPIO_Port GPIOB
#define FLASH_IO0_Pin GPIO_PIN_1
#define FLASH_IO0_GPIO_Port GPIOB
#define GPS_JAM_Pin GPIO_PIN_7
#define GPS_JAM_GPIO_Port GPIOE
#define GPS_3D_FIX_Pin GPIO_PIN_8
#define GPS_3D_FIX_GPIO_Port GPIOE
#define VBUS_ENB_Pin GPIO_PIN_9
#define VBUS_ENB_GPIO_Port GPIOE
#define VBUS_DETECT_Pin GPIO_PIN_10
#define VBUS_DETECT_GPIO_Port GPIOE
#define MagO_INT_Pin GPIO_PIN_11
#define MagO_INT_GPIO_Port GPIOE
#define n_FORCEOFF_Pin GPIO_PIN_12
#define n_FORCEOFF_GPIO_Port GPIOE
#define NXP_SPI2_CS_Pin GPIO_PIN_12
#define NXP_SPI2_CS_GPIO_Port GPIOB
#define NXP_SPI2_SCK_Pin GPIO_PIN_13
#define NXP_SPI2_SCK_GPIO_Port GPIOB
#define NXP_SPI2_DATA_O_Pin GPIO_PIN_14
#define NXP_SPI2_DATA_O_GPIO_Port GPIOB
#define NXP_SPI2_DATA_In_Pin GPIO_PIN_15
#define NXP_SPI2_DATA_In_GPIO_Port GPIOB
#define GPS_TX_Pin GPIO_PIN_10
#define GPS_TX_GPIO_Port GPIOC
#define GPS_RX_Pin GPIO_PIN_11
#define GPS_RX_GPIO_Port GPIOC
#define LTE_U5_TX_Pin GPIO_PIN_12
#define LTE_U5_TX_GPIO_Port GPIOC
#define LTE_U5_RX_Pin GPIO_PIN_2
#define LTE_U5_RX_GPIO_Port GPIOD
#define LTE_U5_RTS_Pin GPIO_PIN_4
#define LTE_U5_RTS_GPIO_Port GPIOB
#define LTE_U5_CTS_Pin GPIO_PIN_5
#define LTE_U5_CTS_GPIO_Port GPIOB
#define MagO_I2C1_SCL_Pin GPIO_PIN_6
#define MagO_I2C1_SCL_GPIO_Port GPIOB
#define MagO_I2C1_SDA_Pin GPIO_PIN_7
#define MagO_I2C1_SDA_GPIO_Port GPIOB
#define LTE_PWR_EN_Pin GPIO_PIN_0
#define LTE_PWR_EN_GPIO_Port GPIOE
#define LTE_ON_OFF_Pin GPIO_PIN_1
#define LTE_ON_OFF_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
#define SOCKET_MAX_RETRIES 5 // A.G. added
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
