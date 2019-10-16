#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#include <applibs/log.h>
#include <applibs/i2c.h>
#include <applibs/gpio.h>

#include "mt3620.h"
#include "thermostat.h"
#include "HDC1080.h"
#include "oled.h"


static int initI2C(void) {

	ISU_2_Fd = I2CMaster_Open(MT3620_ISU2_I2C);
	if (ISU_2_Fd < 0) {
		Log_Debug("ERROR: I2CMaster_Open: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

	int result = I2CMaster_SetBusSpeed(ISU_2_Fd, I2C_BUS_SPEED_FAST);
	if (result != 0) {
		Log_Debug("ERROR: I2CMaster_SetBusSpeed: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

	// Set timeout in miliseconds
	result = I2CMaster_SetTimeout(ISU_2_Fd, 100);
	if (result != 0) {
		Log_Debug("ERROR: I2CMaster_SetTimeout: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}
	return 0;
}

static int initGPIO()
{
	GPIO_relay_Fd = GPIO_OpenAsOutput(10, GPIO_OutputMode_PushPull, GPIO_Value_Low);
	if (GPIO_relay_Fd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}
	return 0;
};

int main(void)
{
	struct HDC1080 HDC1080_sensor;
	struct thermostatSettings userSettings;
	
	initI2C();
	initGPIO();
	HDC1080Begin(&HDC1080_sensor);
	initThermostat(&userSettings, &HDC1080_sensor);
	oled_init();
	

    while (true) {
		//if not away else maintain baseline
		runCycle();
    }
}