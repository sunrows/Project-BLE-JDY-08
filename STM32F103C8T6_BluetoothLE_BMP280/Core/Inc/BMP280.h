/* Date^ 02.01.2020
 * @prefer author  TM CyberPunkTECH®
 * changed and developed by Soliev P.
 */

#ifndef BMP280_H_
#define BMP280_H_

#include <math.h>            // Заголов.файл для математических рассчетов
#include "stm32f1xx_hal.h"   //Библиотека HAL

#define BMP280_dev_address 0xEE

//REGISTR 0xF4
#define F4_osrs_t_skipped			0b00000000
#define F4_osrs_t_oversampling1		0b00100000
#define F4_osrs_t_oversampling2		0b01000000
#define F4_osrs_t_oversampling4		0b01100000
#define F4_osrs_t_oversampling8		0b10000000
#define F4_osrs_t_oversampling16	0b10100000

#define F4_osrs_p_skipped			0b00000000
#define F4_osrs_p_oversampling1		0b00000100
#define F4_osrs_p_oversampling2		0b00001000
#define F4_osrs_p_oversampling4		0b00001100
#define F4_osrs_p_oversampling8		0b00010000
#define F4_osrs_p_oversampling16	0b00010100

#define F4_mode_sleep				0b00000000     //Переход в спящий режим
#define F4_mode_forced				0b00000001     //Быстрая загрузка
#define F4_mode_normal				0b00000011     //Нормальная загрузка

//REGISTR 0xF5 Значения времени, для изменения промежутка снятия значений (Datasheet 19 стр.)
#define F5_t_sb_500us				0b00000000
#define F5_t_sb_62500us				0b00100000
#define F5_t_sb_125ms				0b01000000
#define F5_t_sb_250ms				0b01100000
#define F5_t_sb_500ms				0b10000000
#define F5_t_sb_1sec				0b10100000
#define F5_t_sb_2sec				0b11000000
#define F5_t_sb_4sec				0b11100000

/*Настройки фильтра согласно документации BMP-280*/
#define F5_filter_1					0b00000000
#define F5_filter_2					0b00000010
#define F5_filter_5					0b00000100
#define F5_filter_11				0b00000110
#define F5_filter_22				0b00001010

/*SPI режимы для 4-ех и 3-ех контакных подключений*/
#define F5_spi4w_en					0b00000000
#define F5_spi3w_en					0b00000001

/*I2C обработчик*/
extern I2C_HandleTypeDef hi2c1;

/*Типы данных*/
extern signed long temperature_raw, pressure_raw; // Термометра и давления (влажного)
extern unsigned short dig_T1, dig_P1; //Для выходов (см. Datasheet 21 стр.)
extern signed short dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
extern float temperature, pressure, altitude, init_height; // С плавающей точкой (темп, давл, выс. над. ур. моря,)

extern uint8_t I2C_Read_Register(uint8_t device_adr, uint8_t internal_adr); //функция считывания регистров адресата

extern void I2C_Write_Register(uint8_t device_adr, uint8_t internal_adr, uint8_t data); //функция записи регистров адресата

extern void BMP280_init(uint8_t register_F4, uint8_t register_F5); // функция инициализации регистров F4 и F5 (см. выше)
extern void BMP280_calc_values(void); //функция расчета значений

#endif /* BMP280_H_ */
