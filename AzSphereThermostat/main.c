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
#include "HDC1080.h"

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
}

int main(void)
{
	initI2C();

	struct HDC1080 *HDC1080_sensor;
	HDC1080Begin(HDC1080_sensor);

    //int fd = GPIO_OpenAsOutput(9, GPIO_OutputMode_PushPull, GPIO_Value_High);
    //if (fd < 0) {
    //    Log_Debug(
    //        "Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
    //        strerror(errno), errno);
    //    return -1;
    //}

    const struct timespec sleepTime = {3, 0};
    while (true) {
        /*GPIO_SetValue(fd, GPIO_Value_Low);
        nanosleep(&sleepTime, NULL);
        GPIO_SetValue(fd, GPIO_Value_High);*/
        
		char c[50];
		nanosleep(&sleepTime, NULL);
		if (HDC1080GetTemperature()) {
			sprintf(c, "TEMP -> %f \n", tempC2F());
			Log_Debug(c);
		}

		nanosleep(&sleepTime, NULL);
		if (HDC1080GetHumidity()) {
			sprintf(c, "HUM -> %f \n", HDC1080_sensor->humidity);
			Log_Debug(c);
		}

		
    }
}