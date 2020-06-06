/************************ (C) COPYRIGHT STMicroelectronics *********/
/* Определение для рекурсивных вкючений ------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Заголовочные файлы ------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Экспортируемые прототипы функций-----------------------------------------*/
void Error_Handler(void);
/* Скрытие определения ---------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13 //определения для светодиода PC13
#define LED_GPIO_Port GPIOC // определения для порта С
#define BluetoothReset_Pin GPIO_PIN_14 //определения для PC14 как bluetooth reset
#define BluetoothReset_GPIO_Port GPIOC// определения для bluetooth порта С
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
