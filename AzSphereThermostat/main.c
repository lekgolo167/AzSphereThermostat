#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <curl/curl.h>

#include "applibs_versions.h"
#include <applibs/log.h>
#include <applibs/i2c.h>
#include <applibs/gpio.h>
#include <applibs/rtc.h>
#include <applibs/networking.h>

#include "mt3620.h"
#include "thermostat.h"
#include "HDC1080.h"
#include "HC-SR04.h"
#include "oled.h"
#include "msg.h"

/// <summary>
///     Handle how the ISU2 I2C is setup for the connected devices
/// </summary>
static int initI2C(void) {

	ISU_2_Fd = I2CMaster_Open(MT3620_ISU2_I2C);
	if (ISU_2_Fd < 0) {
		Log_Debug("ERROR: I2CMaster_Open: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

	// Set the bus speed, too slow and the OLED will take too much time to update
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


/// <summary>
///     Sets up all the GPIO inputs/outputs for the device
/// </summary>
static int initGPIO()
{
	
	rotary_A_Fd = GPIO_OpenAsInput(2);
	if (rotary_A_Fd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}
	rotary_B_Fd = GPIO_OpenAsInput(0);
	if (rotary_B_Fd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}
	rotary_SW_Fd = GPIO_OpenAsInput(16);
	if (rotary_SW_Fd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}

	GPIO_relay_Fd = GPIO_OpenAsOutput(35, GPIO_OutputMode_PushPull, GPIO_Value_Low);
	if (GPIO_relay_Fd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}
	buttonAGpioFd = GPIO_OpenAsInput(12);
	if (buttonAGpioFd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}
	buttonBGpioFd = GPIO_OpenAsInput(13);
	if (buttonBGpioFd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}
	motionDetectorGpioFd = GPIO_OpenAsInput(1);
	if (motionDetectorGpioFd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}
	return 0;
};



/// <summary>
///     Print the time in both UTC time zone and local time zone.
/// </summary>
static void PrintTime(void)
{
	// Ask for CLOCK_REALTIME to obtain the current system time. This is not to be confused with the
	// hardware RTC used below to persist the time.
	struct timespec currentTime;
	if (clock_gettime(CLOCK_REALTIME, &currentTime) == -1) {
		Log_Debug("ERROR: clock_gettime failed with error code: %s (%d).\n", strerror(errno),
			errno);
		return;
	}
	else {
		lastMotionDetectedTimeStamp = currentTime.tv_sec;
		char displayTimeBuffer[26];
		if (!asctime_r((gmtime(&currentTime.tv_sec)), (char *restrict) & displayTimeBuffer)) {
			Log_Debug("ERROR: asctime_r failed with error code: %s (%d).\n", strerror(errno),
				errno);
			return;
		}
		Log_Debug("UTC:            %s", displayTimeBuffer);
		if (!asctime_r((localtime(&currentTime.tv_sec)), (char *restrict) & displayTimeBuffer)) {
			Log_Debug("ERROR: asctime_r failed with error code: %s (%d).\n", strerror(errno),
				errno);
			return;
		}

		// Remove the new line at the end of 'displayTimeBuffer'
		displayTimeBuffer[strlen(displayTimeBuffer) - 1] = '\0';
		size_t tznameIndex = ((localtime(&currentTime.tv_sec))->tm_isdst) ? 1 : 0;
		Log_Debug("Local time:     %s %s\n", displayTimeBuffer, tzname[tznameIndex]);
	}
}

int main(void)
{
	struct HDC1080 HDC1080_sensor;
	struct thermostatSettings userSettings;
	userSettings_ptr = &userSettings;
	HDC1080_sensor_ptr = &HDC1080_sensor;

	setenv("TZ", "MST+8MDT", 1);
	tzset();
	PrintTime();

	initI2C();
	initGPIO();
	HDC1080Begin(&HDC1080_sensor);
	initThermostat(&userSettings, &HDC1080_sensor);
	oled_init(&HDC1080_sensor, &userSettings);
	
	update_oled();

	initCycle(&userSettings);
	initTimerEventHandlers();
	
    while (true) {

		WaitForEventAndCallHandler(epollFd);
		if (reconfigureTimer) {
			reconfigureSensorEpollTimer();
			reconfigureTimer = false;
		}
		if (startTemporaryTimer) {
			startTemporaryEpollTimer();
			startTemporaryTimer = false;
		}
    }
}