#include "msg.h"

void sendCURL(char* URLAndPath, char* dataFieldBuffer) {
	CURL *curlHandle = NULL;
	CURLcode res = 0;

	bool isNetworkingReady = false;
	if ((Networking_IsNetworkingReady(&isNetworkingReady) < 0) || !isNetworkingReady) {
		Log_Debug("\nNot doing download because network is not up.\n");
		goto exitLabel;
	}

	Log_Debug("\n -===- Starting upload -===-\n");

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
	}

	exitLabel:
	return;
}

void sendCURLStats(float bt, float bl, float bh, float at, float al, float ah) {

	char path[] = "192.168.0.6:1880/stats";
	char buffer[70];
	sprintf(buffer, "TARGET=%f&THRESH_L=%f&THRESH_H=%f\0", bt, bl, bh);
	sendCURL(path, buffer);

	const struct timespec sleepTime = { 0, 50000000 }; // 50 ms
	nanosleep(&sleepTime, NULL);

	sprintf(buffer, "TARGET=%f&THRESH_L=%f&THRESH_H=%f\0", at, al, ah);
	sendCURL(path, buffer);

	return;
}