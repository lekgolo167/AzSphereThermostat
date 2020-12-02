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
#include "epoll_timerfd_utilities.h"
#include "msg.h"

static int epollFd = -1;

static int buttonPollTimerFd = -1;
static int sensorPollTimerFd = -1;
static int motionPollTimerFd = -1;
static int schedulePollTimerFd = -1;

struct HDC1080 *HDC1080_sensor_ptr;
struct thermostatSettings *userSettings_ptr;

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
///     Handler for the rotary encoder input and buttons A and B
/// </summary>
static void ButtonTimerEventHandler(EventData *eventData)
{
	if (ConsumeTimerFdEvent(buttonPollTimerFd) != 0) {
		return;
	}

	// Get the RTC to update the screen timeout, if buttons are being pressed the screen should stay on 
	struct timespec currentTime;
	clock_gettime(CLOCK_REALTIME, &currentTime);

	//
	///////////////		ROTARY ENCODER CHECK START		/////////////////////

	// Read the rotary encoder A input
	GPIO_Value_Type rotary_A_State = GPIO_Value_High;
	int result = GPIO_GetValue(rotary_A_Fd, &rotary_A_State);
	if (result != 0) {
		Log_Debug("ERROR: Could not read button GPIO: %s (%d).\n", strerror(errno), errno);
		return;
	}

	// Bool used for counting every other turn of the rotary encoder, this way the menu moves one item per 'click' of the rotary encoder as it turns
	static bool docount = true;

	// if the previous state is not equal to the current state, the knob is turning
	if (rotary_A_State != rotary_A_LastState) {

		// Read the rotary encoder B input
		GPIO_Value_Type rotary_B_State = GPIO_Value_High;
		result = GPIO_GetValue(rotary_B_Fd, &rotary_B_State);
		if (result != 0) {
			Log_Debug("ERROR: Could not read button GPIO: %s (%d).\n", strerror(errno), errno);
			return;
		}

		// Each time the knob is turned it counts 2 movments so we need to only check every other time
		if (docount) {

			// This if statment checks which way the knob is turning
			// If A leads B it's CW, if B leads A it's CCW
			if (rotary_B_State != rotary_A_State) {

				// Keep screen on while user interacts by updating a time stamp
				lastMotionDetectedTimeStamp = currentTime.tv_sec; 

				// If in edit mode, keep the scoll counter where it is at and adjust the temporary setting varable shown on the OLED instead
				if (edit_oled_menu) {
					temporary_setting++;
					Log_Debug("Temp: %d\n", temporary_setting);
				}
				else {
					oled_scroll_counter++;
					Log_Debug("Counter: %d\n", oled_scroll_counter);
				}
			}
			else {
				if (edit_oled_menu) {
					temporary_setting--;
					Log_Debug("Temp: %d\n", temporary_setting);
				}	
				else {
					oled_scroll_counter--;
					Log_Debug("Counter: %d\n", oled_scroll_counter);
				}
			}
			// Represh the OLED
			update_oled();	
		}
		docount = !docount; // Toggle so we count every other one
	}
	// Update the state of the rotary encoder
	rotary_A_LastState = rotary_A_State;

	// Check the state of the rotary button/switch; it's active LOW
	GPIO_Value_Type rotary_SW_State = GPIO_Value_High;
	result = GPIO_GetValue(rotary_SW_Fd, &rotary_SW_State);
	if (result != 0) {
		Log_Debug("ERROR: Could not read button GPIO: %s (%d).\n", strerror(errno), errno);
		return;
	}

	if (rotary_SW_LastState != rotary_SW_State) {

		lastMotionDetectedTimeStamp = currentTime.tv_sec; // Keep screen on while user interacts

		rotary_SW_LastState = rotary_SW_State;
		Log_Debug("SW: %d\n", rotary_SW_State);

		if (rotary_SW_State == GPIO_Value_Low) { // If button pressed, edit is true, if pressed again edit is false
			edit_oled_menu = !edit_oled_menu;
			Log_Debug("Edit mode: %d\n", edit_oled_menu);
			// If not editing set the temporary to the corrisponding var
			if (edit_oled_menu == false)
			{
				updateUserSettings();
			}
			else { // If editing, get the corrisponding variable and copy it to a temporary variable
				Log_Debug("temp before: %d\n", temporary_setting);
				updateTemporarySettingValue();
				Log_Debug("temp after: %d\n", temporary_setting);
			}
		}

	}
	//////////////////	END ROTARY ENCODER	////////////////////

	//////////////////	START BUTTON CHECK	////////////////////

	// Check for button A press
	GPIO_Value_Type currentBtnAState;
	result = GPIO_GetValue(buttonAGpioFd, &currentBtnAState);
	if (result != 0) {
		Log_Debug("ERROR: Could not read button GPIO: %s (%d).\n", strerror(errno), errno);
		return;
	}

	// If the A button has just been pressed, send a telemetry message
	// The button has GPIO_Value_Low when pressed and GPIO_Value_High when released
	if (currentBtnAState != lastBtnAState) {

		lastMotionDetectedTimeStamp = currentTime.tv_sec; // keep screen on while user interacts

		// Pressed, not released
		if (currentBtnAState == GPIO_Value_Low) {
			// Change to the next menu
			oled_menu_state++;
			// reset scroll counter so it starts at the top and not where it was before
			oled_scroll_counter = 0;
			Log_Debug("OLED state: %d\n", oled_menu_state);
			// Update OLED to show new menu
			update_oled();
		}

		// Update the static variable to use next time we enter this routine
		lastBtnAState = currentBtnAState;
	}

	// Check for button B press
	GPIO_Value_Type currentBtnBState;
	result = GPIO_GetValue(buttonBGpioFd, &currentBtnBState);
	if (result != 0) {
		Log_Debug("ERROR: Could not read button GPIO: %s (%d).\n", strerror(errno), errno);
		return;
	}

	// If the B button has just been pressed/released, send a telemetry message
	// The button has GPIO_Value_Low when pressed and GPIO_Value_High when released
	if (currentBtnBState != lastBtnBState) {

		lastMotionDetectedTimeStamp = currentTime.tv_sec; // keep screen on while user interacts

		// Pressed, not released
		if (currentBtnBState == GPIO_Value_Low) {
			// Change to the previous menu
			oled_menu_state--;
			// reset scroll counter so it starts at the top and not where it was before
			oled_scroll_counter = 0;
			Log_Debug("OLDED state: %d\n", oled_menu_state);
			// Update OLED to show new menu
			update_oled();
		}

		// Update the static variable to use next time we enter this routine
		lastBtnBState = currentBtnBState;
	}
}

/// <summary>
///     Handler for sensor timer to read temperature and humidity then update the OLED with optional CURL to server
/// </summary>
static void SensorTimerEventHandler(EventData *eventData) {
	if (ConsumeTimerFdEvent(sensorPollTimerFd) != 0) {
		return;
	}
	// Get the temperature and 
	sampleTemperature();
	update_oled();
	
	sprintf(CURLMessageBuffer, "TEMP=%f&HUM=%f\0", averageTemp_F, HDC1080_sensor_ptr->humidity);
	sendCURL(URL_TEMPERATURE, CURLMessageBuffer);

	Log_Debug("Checked Sensors\n");
}

/// <summary>
///     Handler for motion timer to read motion sensor
/// </summary>
static void MotionTimerEventHandler(EventData *eventData) {
	if (ConsumeTimerFdEvent(motionPollTimerFd) != 0) {
		return;
	}
	motionTimer(userSettings_ptr->screenTimeoutSec);
}

/// <summary>
///     Handler for schedule update check timer to update the scheudle from the server
/// </summary>
static void ScheduleServerCheckEventHandler(EventData *eventData) {
	if (ConsumeTimerFdEvent(schedulePollTimerFd) != 0) {
		return;
	}
	checkServerForScheduleUpdates(userSettings_ptr);
}

// event handler data structures. Only the event handler field needs to be populated.
static EventData buttonEventData = { .eventHandler = &ButtonTimerEventHandler };
static EventData sensorEventData = { .eventHandler = &SensorTimerEventHandler };
static EventData motionEventData = { .eventHandler = &MotionTimerEventHandler };
static EventData scheduleEventData = { .eventHandler = &ScheduleServerCheckEventHandler };

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
///     Update the sensor timer if it has been reconfigured
/// </summary>
static int reconfigureSensorEpollTimer() {
	UnregisterEventHandlerFromEpoll(epollFd, sensorPollTimerFd);
	sensorPollTimerFd =
		CreateTimerFdAndAddToEpoll(epollFd, &userSettings_ptr->samplePeriod, &sensorEventData, EPOLLIN);
	if (sensorPollTimerFd < 0) {
		Log_Debug("Failed to create timer\n");
		return -1;
	}
};

/// <summary>
///     Initializes all of the timer event handlers
/// </summary>
static int initTimerEventHandlers() {
	epollFd = CreateEpollFd();
	if (epollFd < 0) {
		return -1;
	}
	struct timespec buttonPressCheckPeriod = { 0, 500000 };
	buttonPollTimerFd =
		CreateTimerFdAndAddToEpoll(epollFd, &buttonPressCheckPeriod, &buttonEventData, EPOLLIN);
	if (buttonPollTimerFd < 0) {
		Log_Debug("Failed to create timer\n");
		return -1;
	}
	sensorPollTimerFd =
		CreateTimerFdAndAddToEpoll(epollFd, &userSettings_ptr->samplePeriod, &sensorEventData, EPOLLIN);
	if (sensorPollTimerFd < 0) {
		Log_Debug("Failed to create timer\n");
		return -1;
	}
	struct timespec motionDetectionPeriod = { 1,0 };
	motionPollTimerFd =
		CreateTimerFdAndAddToEpoll(epollFd, &motionDetectionPeriod, &motionEventData, EPOLLIN);
	if (sensorPollTimerFd < 0) {
		Log_Debug("Failed to create timer\n");
		return -1;
	}

	struct timespec scheduleUpdatePeriod = { 600,0 };
	schedulePollTimerFd =
		CreateTimerFdAndAddToEpoll(epollFd, &scheduleUpdatePeriod, &scheduleEventData, EPOLLIN);
	if (sensorPollTimerFd < 0) {
		Log_Debug("Failed to create timer\n");
		return -1;
	}
	
}

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
    }
}