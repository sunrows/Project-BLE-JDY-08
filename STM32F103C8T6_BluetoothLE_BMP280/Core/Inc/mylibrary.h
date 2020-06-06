/*
 * mylibrary.h
 * author:      Soliev P.
 * Create date: 12.05.2020
 */

#ifndef INC_MYLIBRARY_H_
#define INC_MYLIBRARY_H_

#include "main.h"
#include <string.h>
#include "BMP280.h"

extern char buffer[50];// буфер типа char из массива
extern uint8_t timer_count, buffer_index; //определение типа данных
extern UART_HandleTypeDef huart2;//определение обработчика UART

void ftoa(float n, char *res, int afterpoint);//Для конвертации значений числа с плавающей точкой к массиву строки
uint8_t string_compare(char array1[], char array2[], uint16_t length);//Сравнение строк
void Message_handler();//Обработчик сообщений

#endif /* INC_MYLIBRARY_H_ */
