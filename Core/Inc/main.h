/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

#include "stm32h7xx_ll_adc.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_cortex.h"
#include "stm32h7xx_ll_rcc.h"
#include "stm32h7xx_ll_system.h"
#include "stm32h7xx_ll_utils.h"
#include "stm32h7xx_ll_pwr.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_dma.h"

#include "stm32h7xx_ll_exti.h"

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

void ADC_ConvComplete(void);
void ADC_ConvHalfComplete(void);
void TransferError(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SYS_CLOCK 72000000
#define ADC_TRIG_HZ 10000
#define PWM_HZ 40
#define PWM_TIM_PERIOD 5000

/* USER CODE BEGIN Private defines */

#define  TERMINAL_DEBUG

#define ADC_BUFFER_SIZE						(2*ADC_TRIG_HZ/PWM_HZ)
#define ADC_MAX       						0xFFF

#define LO		0
#define HI		1
#define SET_PIN(PORT,PIN)	PORT->BSRR = (uint32_t)PIN	/// bit set/reset register, 1
#define CLR_PIN(PORT,PIN)	PORT->BRR  = (uint32_t)PIN	/// bit reset register, 0
#define READ_PIN(PORT,PIN)	(PORT->IDR & PIN)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
