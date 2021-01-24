#include "HC-SR04.h"

bool motionTimeoutCheck(unsigned int timeout) {

	struct timespec currentTime;
	clock_gettime(CLOCK_REALTIME, &currentTime);

	long interval = currentTime.tv_sec - lastMotionDetectedTimeStamp;

	Log_Debug("Away Interval: %d - %d\n", interval, timeout);

	if (interval < timeout)
		return true;
	else
		return false;
}

void motionTimer(unsigned int screenTimeout) {
	struct timespec currentTime;
	clock_gettime(CLOCK_REALTIME, &currentTime);

	GPIO_Value_Type motionSensorPin;
	int result = GPIO_GetValue(motionDetectorGpioFd, &motionSensorPin);

	if (motionSensorPin > 0) {
		lastMotionDetectedTimeStamp = currentTime.tv_sec;
	}
	long interval = currentTime.tv_sec - lastMotionDetectedTimeStamp;
	//Log_Debug("Interval: %d\n", interval);
	if (interval > screenTimeout && oledScreenON) {

		oledScreenON = false;

		sprintf(CURLMessageBuffer, "MOTION=%d\0", 0);
		sendCURL(URL_MOTION, CURLMessageBuffer);

		clear_oled_buffer();
		sd1306_refresh();
		Log_Debug("OLED OFF\n");
	}
	else if (interval < screenTimeout && !oledScreenON) {

		oledScreenON = true;

		sprintf(CURLMessageBuffer, "MOTION=%d\0", 1);
		sendCURL(URL_MOTION, CURLMessageBuffer);

		update_oled();
		Log_Debug("OLED ON\n");
	}
}