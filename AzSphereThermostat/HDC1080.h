#pragma once

#define I2C_STRUCTS_VERSION 1
#include <applibs/i2c.h>
#include <applibs/log.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "mt3620.h"

/* ------------------------------------------------------------ */
/*					Reg Addresses								*/
/* ------------------------------------------------------------ */
#define HDC1080_I2C_ADDR   		0x40
#define HDC1080_TMP_REG    		0x00
#define HDC1080_HUM_REG    		0x01
#define HDC1080_CONFIG_REG 		0x02
#define HDC1080_SERIAL_ID_MSB 	0xFB
#define HDC1080_SERIAL_ID_MIDB 	0xFC
#define HDC1080_SERIAL_ID_LSB 	0xFD
#define HDC1080_MANUFACTURER_ID	0xFE
#define HDC1080_DEVICE_ID		0xFF

/* ------------------------------------------------------------ */
/*					Config Register Description					*/
/* ------------------------------------------------------------ */

/*
NAME		BITS		DESCRIPTION				Value
-----------------------------------------------------------------------------------------------------
RST			[15]		software reset bit		0	normal operation, self clears
												1	software reset
-----------------------------------------------------------------------------------------------------
RESERVED	[14]		reserved				0	reserved, must be zero
-----------------------------------------------------------------------------------------------------
HEAT		[13]		heater					1	heater enabled
												0	heater disabled
-----------------------------------------------------------------------------------------------------
MODE		[12]		mode of aquisition		0 	temp or hum is acquired
												1 	temp and hum are aquired in sequence, Temp first
-----------------------------------------------------------------------------------------------------
BTST		[11]		battery status			0	battery voltage > 2.8V (read only)
												1 	battery voltage < 2.8V (read only)
-----------------------------------------------------------------------------------------------------
TRES		[10]		temp measurement		0	14-bit
						resolution				1	11-bit
-----------------------------------------------------------------------------------------------------
HRES		[9:8]		humidity measure-		00	14-bit
						ment resolution			01	11-bit
												10	8-bit
-----------------------------------------------------------------------------------------------------
RESERVED	[7:0]		reserved				0	reserved, must be zero
-----------------------------------------------------------------------------------------------------
*/

/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

struct HDC1080 {
	float humidity;
	float temp_C;
	float temp_F;
};

uint16_t ByteTo2Bytes(const uint8_t* raw_data);
bool CheckTransferSize(const char *desc, size_t expectedBytes, ssize_t actualBytes);

bool HDC1080GetTemperature();
bool HDC1080GetHumidity();
void  HDC1080Begin(struct HDC1080* hdc1080_ptr);

float tempC2F();
float tempF2C(float deg_F);