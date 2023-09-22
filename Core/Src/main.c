/* USER CODE BEGIN Header */
/**************************************************************************//**
 * @file main.c
 * @brief This code contains most of the task
 * @version V5.38.0.0
 * @date 22.09.2023
 * @author Maksim Abramov
 * \mainpage Descriptiion
 * Implementation of ADC input reading using DMA with 10 KHz sampling and setting of received PWM output averages 
 * once every 25 milliseconds (pulse length is proportional to ADC code, maximum ADC value corresponds to maximum pulse duration). 
 * As well as the output of 1 second averaged values on the SWD interface to the IDE Keil terminal. 
 * The obtained values are displayed in millivolts, the format of the representation in ASCII codes.
 * The project is generated in STM32CubeMX for Keil environment using "LL" libraries for ADC, for the rest of periphery on "HAL".
 *\section Detail
 *Board NUCLEO-H743ZI
*
******************************************************************************/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "math.h"
#include "stdio.h"
#ifdef TERMINAL_DEBUG
//#include "EventRecorder.h"
#endif

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint16_t adc_buffer[ADC_BUFFER_SIZE];	///< Buffer for measured ADC values
uint32_t adcAvg;  ///< Average value measured by the ADC
const float conv_coef = (float)PWM_TIM_PERIOD/ADC_MAX; ///< Coefficient of conversion from ADC code to PWM pulse

#ifdef TERMINAL_DEBUG
	#define TERM_MAX_COUNTER	40
	float mcuVoltage_mV=0;  ///< Internal reference voltage VDDA in mV
	float adcVoltage_mV=0;	///< Absolute voltage value of ADC channel in mV
	uint8_t term_adc_counter=TERM_MAX_COUNTER;
	uint32_t term_adc_code_summ=0;
#endif

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Initialize(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
#ifdef TERMINAL_DEBUG		
	//EventRecorderInitialize( EventRecordAll, 1 );
#endif	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* USER CODE BEGIN Init */

	HAL_SuspendTick(); //> Disable SysTick Interrupt
	
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_ADC3_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	
	LL_ADC_StartCalibration(ADC3,LL_ADC_CALIB_OFFSET,LL_ADC_SINGLE_ENDED);
	
	LL_ADC_Enable(ADC3);
	
	LL_ADC_REG_SetDataTransferMode(ADC3, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

	LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_0, ADC_BUFFER_SIZE);
	LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_0, (uint32_t)&ADC3->DR,
                         (uint32_t)adc_buffer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_0);
	LL_DMA_EnableIT_HT(DMA1, LL_DMA_STREAM_0);
  LL_DMA_EnableIT_TE(DMA1, LL_DMA_STREAM_0);

	LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);
	
	LL_ADC_REG_StartConversion(ADC3);
	
	LL_ADC_INJ_StartConversion(ADC3);  ///< For calculating the actual VDDA voltage using the internal reference voltage
	
	HAL_TIM_Base_Start_IT(&htim3); ///< ADC trigger 10KHz

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); ///< PWM 40Hz

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 18;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

#ifdef TERMINAL_DEBUG
/**
  * @brief  Calculate average ADC value and send it in terminal by SWD interface 
  * @param[in] adcCode Value from ADC
  * @retval None
  */
void Term_ADC_Iteration(uint32_t *adcCode)
{
	term_adc_code_summ += *adcCode;
	if( --term_adc_counter == 0 ){
		mcuVoltage_mV = __LL_ADC_CALC_VREFANALOG_VOLTAGE(LL_ADC_INJ_ReadConversionData12(ADC3, LL_ADC_INJ_RANK_1),LL_ADC_RESOLUTION_12B);
		LL_ADC_INJ_StartConversion(ADC3);
		adcVoltage_mV = (float)term_adc_code_summ / TERM_MAX_COUNTER * mcuVoltage_mV / ADC_MAX;
		term_adc_code_summ = 0;
		term_adc_counter = TERM_MAX_COUNTER;
		
		printf("%.0f mV\r\n",adcVoltage_mV);
	}
}
#endif

/**
  * @brief  Conversion DMA transfer callback
  * @param  None
  * @retval None
  */
void ADC_ConvHalfComplete()
{
	adcAvg=0;
	for(int i=0; i<ADC_BUFFER_SIZE/2; ++i){
		adcAvg += adc_buffer[i]; 
	}
	adcAvg /= ADC_BUFFER_SIZE/2;

	TIM2->CCR1 = floor(conv_coef * adcAvg  + 0.5);
	
#ifdef TERMINAL_DEBUG
	Term_ADC_Iteration(&adcAvg);
#endif
}

/**
  * @brief  Conversion DMA half-transfer callback
  * @param  None
  * @retval None
  */
void ADC_ConvComplete()
{
	adcAvg=0;
	for(int i=ADC_BUFFER_SIZE/2; i<ADC_BUFFER_SIZE; ++i){
		adcAvg += adc_buffer[i]; 
	}
	adcAvg /= ADC_BUFFER_SIZE/2;
	
	TIM2->CCR1 = floor(conv_coef * adcAvg  + 0.5);
	
#ifdef TERMINAL_DEBUG
	Term_ADC_Iteration(&adcAvg);
#endif	
}

/**
  * @brief  DMA transfer error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
void TransferError()
{
  /* Error detected during DMA transfer */
	
}

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
