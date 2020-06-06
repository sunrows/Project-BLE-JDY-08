/*
 * mylibrary.c
 * Create: 12.03.2020
 * Author: Soliev P.
 */
#include "mylibrary.h"

char buffer[50];
uint8_t timer_count = 0, buffer_index = 0;

// Реверсирование строки 'str' в длину 'len'
void reverse(char *str, int len)
{
	int i = 0, j = len - 1, temp;
	while (i < j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}

/*
 * Преобразование данного целого числа x в строку str[]
 * "d" это число значений, необходимых для вывода. Если "d"
 * больше, чем число значений "x", тогда в начале добавляется "0"
*/
/*Преобразование числа в строку*/
int intToStr(int x, char str[], int d)
{
	int i = 0;
	while (x)
	{
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}

/*Если число "d" больше чем значения "x" тогда в начале добавляется "0" */
	while (i < d)
		str[i++] = '0';

	reverse(str, i);
	str[i] = '\0';
	return i;
}

/*Преобразование числа с плавающей точкой в строку str[]*/
void ftoa(float n, char *res, int afterpoint)
{
	unsigned char minus_flag = 0;
	if (n < 0)
	{
		minus_flag = 1;
		n = -n;
	}

	// Извлечение целочисленной части
	int ipart = (int)n;

	// Извлечение дробной части
	float fpart = n - (float)ipart;

	// Преобразование целого числа в str[]
	int i = intToStr(ipart, res, 0);

	//Проверка  вариантов отображения после точки.
	if (afterpoint != 0)
	{
		res[i] = '.';  /* добавить точку,
						*@получение значение дробной части до заданного "нет".
						*@из точек после точки. нужен третий параметр
						*@обрабатывающий такие числа как 122.009*/
		fpart = fpart * pow(10, afterpoint);

		intToStr((int)fpart, res + i + 1, afterpoint);
	}

	char string[30];
	if (minus_flag == 1)
	{
		memset(string, 0, 30);
		string[0] = '-';
		if (n < 1.0f)
		{
			string[1] = '0';
			strcpy(&string[2], res);
		}
		else
			strcpy(&string[1], res);

		memset(res, 0, strlen(res));
		strcpy(res, string);
	}
	else
		if (n < 1.0f)
		{
			string[0] = '0';
			strcpy(&string[1], res);
			memset(res, 0, strlen(res));
			strcpy(res, string);
		}

}
/*Сравнение строк */
uint8_t string_compare(char array1[], char array2[], uint16_t length)
{
	 uint8_t comVAR=0, i;
	 for(i=0;i<length;i++)
	   	{
	   		  if(array1[i]==array2[i])
	   	  		  comVAR++;
	   	  	  else comVAR=0;
	   	}
	 if (comVAR==length)
		 	return 1;
	 else 	return 0;
}
/*Функция обработчика сообщений*/
void Message_handler()
{
	if(string_compare(buffer, "get", strlen("get")))
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);
		BMP280_calc_values();
		char string[50];
		memset(string, 0, sizeof(string));
		ftoa(temperature, &string[0], 3);
		strcat(string, ",");
		ftoa(pressure, &string[strlen(string)], 3);
		strcat(string, ",");
		ftoa(altitude, &string[strlen(string)], 3);
		strcat(string, "\n");
		HAL_UART_Transmit(&huart2, (uint8_t*)string, strlen(string), 500);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
	}

	memset(buffer, 0, sizeof(buffer));
	buffer_index = 0;
	timer_count = 0;
}
