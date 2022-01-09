/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * 4янв2022
  * @Biriuk
  * peotr60@mail.ru
  *
  * RTOS_1
  * Изучение операционной среды FreeRTOS в среде STM32CubeIDE с помощью библиотеки HAL
  * с использованием отладочной платы NUCLEO-L452RE-P
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  * 4янв22
  * ВНИМАНИЕ!
  * Столкнулся с проблемой при миграции stm32cube_fw_l4_v1170 на stm32cube_fw_l4_v1171 в проекте с RTOS. После обновления проекта в RTOS.ioc и компилировании main.c выдавалось 9 ошибок следующего характера:
  * multiple definition of `SystemCoreClockUpdate'; ./Core/Src/system_stm32l4xx.o:
  * и далее путь D:/STM32/STM32_File/RTOS_1/Debug/../Core/Src/system_stm32l4xx.c:272: first defined here
  * С трудом нашёл вот это:
  * https://stackoverflow.com/questions/64090730/multiple-definition-of-first-defined-here-stm32-ac6-studio
  * Вот перевод:
  * Это ошибки компоновщика. Вы, вероятно, делаете что - то подобное:
  * главная.c:
  * #include "lib.h"
  * init.c:
  * #include "lib.h"
  * Когда компоновщик проверяет символы и типы в main.c и init.c, он обнаруживает, что в каждом из них были определены одни и те же символы, следовательно, ошибка. Обычно заголовочные файлы только объявляют символы, а не определяют их, но есть некоторые творческие способы использования условного кода, которые могут обойти это
  * Т.е. производится повторное определение или включение одного и того же. В моём случае оказалось, что файл system_stm32l4xx.c находится в двух папках:
  * D:\STM32\STM32_File\RTOS_1\Core\Src\system_stm32l4xx.c
  * D:\STM32\STM32_File\RTOS_1\Drivers\CMSIS\Device\ST\STM32L4xx\Source\Templates\Core\Src\system_stm32l4xx.c
  * После удаления файла из Templates ошибки исчезли! Однако, после последующего обновления проекта в RTOS.ioc указанный файл в папке Templates восстановился, и вновь выдались ошибки!
  * Удаления из папки Core\Src\ приводили к аналогичным результатам.
  * Т.е. нужно после каждого обновления проекта в окне, вызываемом файлом *.ioc (в моём случае это RTOS.ioc) нужно удалять повторяющиеся файлы, в моём случае файл system_stm32l4xx.c. Лучше удалять из папки Templates И СЛЕДИТЬ ЗА ОПЕРАТОРАМИ В main.c, могут пропадать после такого обновления. У меня пропадали файлы:
  * #include "string.h" // это для функции strlen()
  * #include <stdio.h>
  * N.B! Чтобы инклюды не пропадали, всего-навсего нужно было их вводить в графу "Private includes"!
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h" // это для функции strlen()
#include <stdio.h>

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
UART_HandleTypeDef huart2;

osThreadId defaultTaskHandle;
osThreadId myLedTaskHandle;
osThreadId myBtnHandle;
/* USER CODE BEGIN PV */
char trans_str[64] = {0,};
int LED_State = 0;
int T_LED = 100;
int Btn_State = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument);
void StartLedTask(void const * argument);
void StartBtn(void const * argument);

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

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myLedTask */
  osThreadDef(myLedTask, StartLedTask, osPriorityNormal, 0, 128);
  myLedTaskHandle = osThreadCreate(osThread(myLedTask), NULL);

  /* definition and creation of myBtn */
  osThreadDef(myBtn, StartBtn, osPriorityNormal, 0, 128);
  myBtnHandle = osThreadCreate(osThread(myBtn), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
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
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : Btn1_Pin */
  GPIO_InitStruct.Pin = Btn1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Btn1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD4_Pin */
  GPIO_InitStruct.Pin = LD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD4_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//это стандартна¤ запись для посимвольного вывода информации в интерфейс ITM в среде STM32CubeIDE
//(должно быть в случае применени¤ SWO)
//https://www.youtube.com/watch?v=nE-YrKpWjso&list=PL9lkEHy8EJU8a_bqiJXwGTo-uM_cPa98P
int __io_putchar(int ch)
{
	ITM_SendChar(ch);
	return ch;
}

//или:

//STM32: отладка через SWO в STM32CubeIDE с доработкой ST-LINK
//https://www.youtube.com/watch?v=ST_fUu6ACzE
//https://www.youtube.com/watch?v=iC2-0Md-6yg

//int _write(int file, char *ptr, int len)
 //{
 //int i = 0;
 //for(i = 0; i < len; i++)
    //ITM_SendChar((*ptr++));
 //return len;
 //}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartLedTask */
/**
* @brief Function implementing the myLedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLedTask */
void StartLedTask(void const * argument)
{
  /* USER CODE BEGIN StartLedTask */
	/* Infinite loop */
	for (;;)
	{
		HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
		LED_State = HAL_GPIO_ReadPin(LD4_GPIO_Port, LD4_Pin);

		//snprintf в stm32:
		//https://eax.me/stm32-spi-flash/
		//ПРОВЕРИТЬ вариант:
		snprintf(trans_str, sizeof(trans_str), "UART: RTOS  работает правильно! \n\r");
		//https://istarik.ru/blog/stm32/113.html
		//snprintf(trans_str, 63, "RTOS работает правильно! UART LED_State\n\r");
		HAL_UART_Transmit(&huart2, (uint8_t*) trans_str, strlen(trans_str),100);
		printf("RTOS (printf): режим LED\r");
		puts("RTOS (puts): режим LED\r");

		//osDelay(200);
		osDelay(T_LED);
	}
  /* USER CODE END StartLedTask */
}

/* USER CODE BEGIN Header_StartBtn */
/**
* @brief Function implementing the myBtn thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBtn */
void StartBtn(void const * argument)
{
  /* USER CODE BEGIN StartBtn */
	/* Infinite loop */
	for (;;)
	{
		//HAL_GPIO_Init(Btn1_GPIO_Port, &GPIO_InitStruct);
		//LED_State = HAL_GPIO_ReadPin(LD4_GPIO_Port, LD4_Pin);

		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)==GPIO_PIN_RESET)
			//Почему-то GPIO_PIN_RESET вместо GPIO_PIN_SET? На схеме нажатие приводит к "1"
		{
		while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)==GPIO_PIN_RESET)
		{
			vTaskDelay(50);
		}  //антидребезг
		T_LED=250;
		//Btn_State = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13);
		//snprintf в stm32:
		//https://eax.me/stm32-spi-flash/
		snprintf(trans_str, sizeof(trans_str), "UART: RTOS, Нажата кнопка!\n\r");
		HAL_UART_Transmit(&huart2, (uint8_t*) trans_str, strlen(trans_str),100);
		printf("RTOS (printf): Нажата кнопка!\r");
		puts("RTOS (puts): Нажата кнопка!\r");

		//printf("RTOS arbeit richtig!  21");
		puts("RTOS (puts): Nazhata knopka!\r");
		}

		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)==GPIO_PIN_SET)
		{
		while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)==GPIO_PIN_SET)
		{
			vTaskDelay(50);
		}	  //антидребезг
		T_LED=1000;
		//Btn_State = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13);
		}

		Btn_State = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13);

		osDelay(10);
	}
  /* USER CODE END StartBtn */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
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

