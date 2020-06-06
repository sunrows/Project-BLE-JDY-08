/* Файлы заголовки */
#include "main.h"

#include "mylibrary.h"
/* Скрытые переменные */
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;
/* Примеры скрытых функций определяемые STM32CubeMX */
void SystemClock_Config(void); // инициализация системного тактирования
static void MX_GPIO_Init(void); //инициализация портов ввода вывода
static void MX_USART2_UART_Init(void);// инициализация UART
static void MX_I2C1_Init(void);//инициализация I2C
static void MX_TIM2_Init(void);//инициализация TIM2
int main(void)
{
  /* Перезагрузка всех периферийных устройств*/
  HAL_Init();

  /* конфигурация системного тактирования */
  SystemClock_Config();

  /* Инициализация всех периферийных устройств*/
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  /* Начало кода программы */
  HAL_Delay(200);
  HAL_GPIO_WritePin(BluetoothReset_GPIO_Port, BluetoothReset_Pin, SET); //лог. единица BLE модуля-выхода RESET
  HAL_Delay(1000);
  HAL_UART_Transmit(&huart2, (uint8_t*)"AT+NAMEBMP280", strlen("AT+NAMEBMP280"), 500);// функция HAL по работе с UART
  memset(buffer, 0, sizeof(buffer));//заполнения массива buffer указанными символами (0), указанным размером
  HAL_TIM_Base_Start_IT(&htim2);
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  /*Функция для инициализации отбора значений (побитово) для BMP280 (см. BMP280.h)*/
  BMP280_init(F4_osrs_t_oversampling16|
			  F4_osrs_p_oversampling16|
			  F4_mode_normal,
			  F5_t_sb_500us|
			  F5_filter_22|
			  F5_spi4w_en);

  /* Бесконечный цикл от STM32 Cube MX*/
  while (1)
  {
  }
}

/**
  * @ настройки системы тактирования генерируемое STM32Cube MX
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /*Инициализация RCC шин CPU, APB и AHB  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;

  /*Цикл для проверки структуры запуска тактирования сист.и периф.RCC*/
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* Инициализация шин CPU, APB, AHB*/
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
/*Цикл для проверки запуска систем тактирования*/
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  *Функция инициализации I2C1 генерирован STM32Cube MX*/
static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  /*Цикл для проверки инициализации I2C*/
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}

/*Функция инициализации TIM2 генерирован STM32Cube MX*/
static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 720;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  /*Цикл для проверки инициализации TIM2*/
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  /*Цикл для проверки инициализации источника тактирования*/
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @Инициализация UART2 генерирован STM32CubeMX
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  /*Функция проверки инициализации UART2*/
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/*Инициализаци вход\выход (GPIO) генерирован STM32Cube MX */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Включение тактирования в GPIO */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Настройки уровня выхода GPIO*/
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

  /*Настройки уровня выхода GPIO для Bluetooth RESET*/
  HAL_GPIO_WritePin(BluetoothReset_GPIO_Port, BluetoothReset_Pin, GPIO_PIN_RESET);

  /*Настройки GPIO ножек: LED_Pin BluetoothReset_Pin */
  GPIO_InitStruct.Pin = LED_Pin|BluetoothReset_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}
/*Функция обработчика ошибок  */
void Error_Handler(void)
{ }

/*Конец файлов обработки утверждений генерирован STM32CubeMX*/
#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{ }
#endif /* USE_FULL_ASSERT */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
