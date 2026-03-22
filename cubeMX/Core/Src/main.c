/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

// Includes ------------------------------------------------------------------
#include "main.h"

// includes ------------------------------------------------------------------

// typedef -------------------------------------------------------------------

// define --------------------------------------------------------------------

// macro ---------------------------------------------------------------------

#define TRY(code)  if (code != HAL_OK) { Error_Handler(); }

// variables -----------------------------------------------------------------
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim21;
UART_HandleTypeDef huart2;

// function prototypes -------------------------------------------------------
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM21_Init(void);
static void MX_USART2_UART_Init(void);

#ifdef __cplusplus
extern "C" {
#endif

void CTRL_application_main(void);  // forward declaration

#ifdef __cplusplus
}
#endif

// user code -----------------------------------------------------------------

// @brief  The application entry point.
// @retval int
int main(void) {

	// MCU Configuration--------------------------------------------------------

	HAL_Init();             // Reset of all peripherals, Initializes the Flash interface and the Systick.
	SystemClock_Config();   // Configure the system clock

	// Initialize all configured peripherals
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_TIM2_Init();
	MX_TIM21_Init();
	MX_USART2_UART_Init();

	CTRL_application_main();
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	// Configure the main internal regulator output voltage
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	// Initializes the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure.
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	TRY(HAL_RCC_OscConfig(&RCC_OscInitStruct))

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
								|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	TRY(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0))

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
	TRY(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit))
}


// @brief I2C1 Initialization Function
// @param None
// @retval None
static void MX_I2C1_Init(void) {

	hi2c1.Instance = I2C1;
	hi2c1.Init.Timing = 0x00000608;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	TRY(HAL_I2C_Init(&hi2c1))

	// Configure Analogue filter
	TRY(HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE))

	// Configure Digital filter
	TRY(HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0))
}


// @brief TIM2 Initialization Function
// @param None
// @retval None
static void MX_TIM2_Init(void) {

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 65535;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	TRY(HAL_TIM_Base_Init(&htim2))

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	TRY(HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig))

	TRY(HAL_TIM_PWM_Init(&htim2))

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	TRY(HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig))

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	TRY(HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1))

	HAL_TIM_MspPostInit(&htim2);

}


// @brief TIM21 Initialization Function
// @param None
// @retval None
static void MX_TIM21_Init(void) {

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};


	htim21.Instance = TIM21;
	htim21.Init.Prescaler = 0;
	htim21.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim21.Init.Period = 65535;
	htim21.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim21.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	TRY(HAL_TIM_Base_Init(&htim21))
	
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	TRY(HAL_TIM_ConfigClockSource(&htim21, &sClockSourceConfig))
	
	TRY(HAL_TIM_PWM_Init(&htim21))
	
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	TRY(HAL_TIMEx_MasterConfigSynchronization(&htim21, &sMasterConfig))
	
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	TRY(HAL_TIM_PWM_ConfigChannel(&htim21, &sConfigOC, TIM_CHANNEL_2))

	HAL_TIM_MspPostInit(&htim21);

}


// @brief USART2 Initialization Function
// @param None
// @retval None
static void MX_USART2_UART_Init(void) {

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	TRY(HAL_UART_Init(&huart2))

}


// @brief GPIO Initialization Function
// @param None
// @retval None
static void MX_GPIO_Init(void) {

	// GPIO Ports Clock Enable
	__HAL_RCC_GPIOA_CLK_ENABLE();

}


// @brief  This function is executed in case of error occurrence.
// @retval None
void Error_Handler(void) {

	// User can add his own implementation to report the HAL error return state
	__disable_irq();
	while (1)
	{
	}
}


#ifdef USE_FULL_ASSERT

// @brief  Reports the name of the source file and the source line number
//         where the assert_param error has occurred.
// @param  file: pointer to the source file name
// @param  line: assert_param error line source number
// @retval None
void assert_failed(uint8_t *file, uint32_t line)
{
  	// User can add his own implementation to report the file name and line number,
    // ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)
}

#endif /* USE_FULL_ASSERT */
