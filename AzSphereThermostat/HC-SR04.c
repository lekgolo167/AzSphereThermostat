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

		char path[] = "192.168.0.6:1880/motion";
		char buffer[] = "MOTION=1\0";
		sendCURL(path, buffer);

		oledScreenON = false;
		const struct timespec sleepTime = { 0, 50000000 }; // 50 ms
		nanosleep(&sleepTime, NULL);

		buffer[7] = '0';
		sendCURL(path, buffer);

		clear_oled_buffer();
		sd1306_refresh();
		Log_Debug("OLED OFF\n");
	}
	else if (interval < screenTimeout && !oledScreenON) {

		char path[] = "192.168.0.6:1880/motion";
		char buffer[] = "MOTION=0\0";
		sendCURL(path, buffer);

		oledScreenON = true;
		const struct timespec sleepTime = { 0, 50000000 }; // 50 ms
		nanosleep(&sleepTime, NULL);

		buffer[7] = '1';
		sendCURL(path, buffer);

		update_oled();
		Log_Debug("OLED ON\n");
	}
}