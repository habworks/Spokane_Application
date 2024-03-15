/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    octospi.c
  * @brief   This file provides code for the configuration
  *          of the OCTOSPI instances.
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
/* Includes ------------------------------------------------------------------*/
#include "octospi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

OSPI_HandleTypeDef hospi1;
DMA_HandleTypeDef hdma_octospi1;

/* OCTOSPI1 init function */
void MX_OCTOSPI1_Init(void)
{

  /* USER CODE BEGIN OCTOSPI1_Init 0 */

  /* USER CODE END OCTOSPI1_Init 0 */

  OSPIM_CfgTypeDef OSPIM_Cfg_Struct = {0};

  /* USER CODE BEGIN OCTOSPI1_Init 1 */

  /* USER CODE END OCTOSPI1_Init 1 */
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold = 1;
  hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;
  hospi1.Init.DeviceSize = 27;
  hospi1.Init.ChipSelectHighTime = 2;
  hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi1.Init.ClockPrescaler = 60;
  hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_DISABLE;
  hospi1.Init.ChipSelectBoundary = 0;
  hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
  OSPIM_Cfg_Struct.ClkPort = 1;
  OSPIM_Cfg_Struct.NCSPort = 1;
  OSPIM_Cfg_Struct.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
  if (HAL_OSPIM_Config(&hospi1, &OSPIM_Cfg_Struct, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */

  /* USER CODE END OCTOSPI1_Init 2 */

}

void HAL_OSPI_MspInit(OSPI_HandleTypeDef* ospiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(ospiHandle->Instance==OCTOSPI1)
  {
  /* USER CODE BEGIN OCTOSPI1_MspInit 0 */

  /* USER CODE END OCTOSPI1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
    PeriphClkInit.OspiClockSelection = RCC_OSPICLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* OCTOSPI1 clock enable */
    __HAL_RCC_OSPIM_CLK_ENABLE();
    __HAL_RCC_OSPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**OCTOSPI1 GPIO Configuration
    PA2     ------> OCTOSPIM_P1_NCS
    PA3     ------> OCTOSPIM_P1_CLK
    PA6     ------> OCTOSPIM_P1_IO3
    PA7     ------> OCTOSPIM_P1_IO2
    PB0     ------> OCTOSPIM_P1_IO1
    PB1     ------> OCTOSPIM_P1_IO0
    */
    GPIO_InitStruct.Pin = FLASH_CS_Pin|FLASH_CLK_Pin|FLASH_IO3_Pin|FLASH_IO2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = FLASH_IO1_Pin|FLASH_IO0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* OCTOSPI1 DMA Init */
    /* OCTOSPI1 Init */
    hdma_octospi1.Instance = DMA2_Channel4;
    hdma_octospi1.Init.Request = DMA_REQUEST_OCTOSPI1;
    hdma_octospi1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_octospi1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_octospi1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_octospi1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_octospi1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_octospi1.Init.Mode = DMA_NORMAL;
    hdma_octospi1.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_octospi1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(ospiHandle,hdma,hdma_octospi1);

    /* OCTOSPI1 interrupt Init */
    HAL_NVIC_SetPriority(OCTOSPI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(OCTOSPI1_IRQn);
  /* USER CODE BEGIN OCTOSPI1_MspInit 1 */

  /* USER CODE END OCTOSPI1_MspInit 1 */
  }
}

void HAL_OSPI_MspDeInit(OSPI_HandleTypeDef* ospiHandle)
{

  if(ospiHandle->Instance==OCTOSPI1)
  {
  /* USER CODE BEGIN OCTOSPI1_MspDeInit 0 */

  /* USER CODE END OCTOSPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_OSPIM_CLK_DISABLE();
    __HAL_RCC_OSPI1_CLK_DISABLE();

    /**OCTOSPI1 GPIO Configuration
    PA2     ------> OCTOSPIM_P1_NCS
    PA3     ------> OCTOSPIM_P1_CLK
    PA6     ------> OCTOSPIM_P1_IO3
    PA7     ------> OCTOSPIM_P1_IO2
    PB0     ------> OCTOSPIM_P1_IO1
    PB1     ------> OCTOSPIM_P1_IO0
    */
    HAL_GPIO_DeInit(GPIOA, FLASH_CS_Pin|FLASH_CLK_Pin|FLASH_IO3_Pin|FLASH_IO2_Pin);

    HAL_GPIO_DeInit(GPIOB, FLASH_IO1_Pin|FLASH_IO0_Pin);

    /* OCTOSPI1 DMA DeInit */
    HAL_DMA_DeInit(ospiHandle->hdma);

    /* OCTOSPI1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(OCTOSPI1_IRQn);
  /* USER CODE BEGIN OCTOSPI1_MspDeInit 1 */

  /* USER CODE END OCTOSPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
