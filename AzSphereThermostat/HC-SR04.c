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

		char buffer2[] = "MOTION=0\0";
		sendCURL(path, buffer2);

		clear_oled_buffer();
		sd1306_refresh();
		Log_Debug("OLED OFF\n");
	}
	else if (interval < screenTimeout && !oledScreenON) {

		char path[] = "192.168.0.6:1880/motion";
		char buffer[] = "MOTION=0\0";
		sendCURL(path, buffer);

		oledScreenON = true;

		char buffer2[] = "MOTION=1\0";
		sendCURL(path, buffer2);

		update_oled();
		Log_Debug("OLED ON\n");
	}
}

static void sendCURL(char* URLAndPath, char* dataFieldBuffer) {
	CURL *curlHandle = NULL;
	CURLcode res = 0;

	bool isNetworkingReady = false;
	if ((Networking_IsNetworkingReady(&isNetworkingReady) < 0) || !isNetworkingReady) {
		Log_Debug("\nNot doing download because network is not up.\n");
		goto exitLabel;
	}

	Log_Debug("\n -===- Starting download -===-\n");

	// Init the cURL library.
	if ((res = curl_global_init(CURL_GLOBAL_ALL)) != CURLE_OK) {
		Log_Debug("curl_global_init");
		goto exitLabel;
	}

	if ((curlHandle = curl_easy_init()) == NULL) {
		Log_Debug("curl_easy_init() failed\n");
		goto exitLabel;
	}

	// Specify URL to download.
	// Important: any change in the domain name must be reflected in the AllowedConnections
	// capability in app_manifest.json.
	if ((res = curl_easy_setopt(curlHandle, CURLOPT_URL, URLAndPath)) != CURLE_OK) {
		Log_Debug("curl_easy_setopt CURLOPT_URL");
		goto exitLabel;
	}

	// Set output level to verbose.

	if ((res = curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, dataFieldBuffer)) != CURLE_OK) {
		Log_Debug("curl_easy_setopt CURLOPT_VERBOSE");
		goto exitLabel;
	}

	// Perform the download of the web page.
	if ((res = curl_easy_perform(curlHandle)) != CURLE_OK) {
		Log_Debug(" -===- FAILED -===- \n");
	}
	else {
		curl_easy_cleanup(curlHandle);
		// Clean up cURL library's resources.
		curl_global_cleanup();
		Log_Debug("\n -===- Done Uploading -===-\n");
	}

exitLabel:
	return;
}