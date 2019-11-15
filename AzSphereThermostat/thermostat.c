#include "thermostat.h"
#include "oled.h"

struct thermostatSettings *userSettings;
struct HDC1080 *HDC1080_sensor;

float temperatureSamples[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int sampleAverageIndex = 0;
long furnaceStartTime = 0;
long furnaceStopTime = 0;
long furnaceRunTime = 0;

void initThermostat(struct thermostatSettings *userSettings_ptr, struct HDC1080 *HDC1080_sensor_ptr)
{
	userSettings = userSettings_ptr;

	HDC1080_sensor = HDC1080_sensor_ptr;
	const struct timespec sleeptime = { 0, 50000000 };
	while (!HDC1080GetTemperature()) {
		Log_Debug("failed to get temperature\n");
		nanosleep(&sleeptime, NULL);
	};

	Log_Debug("initializing sample arrary with: %f\n", HDC1080_sensor_ptr->temp_C);
	for (int i = 0; i < 20; i++) {
		temperatureSamples[i] = HDC1080_sensor_ptr->temp_C;
	}
};

void runCycle(float roomTemp_C)
{
	int8_t state = -1;
	bool checklist = preRunChecklist();

	// Check if room is below target temperature
	if (checklist && roomTemp_C <= (userSettings->targetTemp_C - userSettings->lower_threshold))
	{
		Log_Debug("[INFO:] Below target\n");
		// Run furnace until room reaches target
		state = true;
	}
	// Check if room is above target temperature
	else if (checklist && roomTemp_C >= (userSettings->targetTemp_C + userSettings->upper_threshold))
	{
		Log_Debug("[INFO:] Above target\n");
		state = false;
	}
	// Check if room is below baseline temperature
	else if (roomTemp_C <= (userSettings->baselineTemp_C - userSettings->lower_threshold)) {
		// Run furnace until room reaches baseline
		Log_Debug("[INFO:] Below baseline\n");
		state = true;
	}
	// check if room is above baseline temperature
	else if (!checklist && roomTemp_C >= (userSettings->baselineTemp_C + userSettings->upper_threshold))
	{
		Log_Debug("[INFO:] Above baseline\n");
		state = false;
	}

	if (state >= 0) {
		Log_Debug("relay %d\n", state);
		furnaceRelay(state);
	}

};

float sampleTemperature()
{
	HDC1080GetTemperature();
	HDC1080GetHumidity();

	temperatureSamples[sampleAverageIndex++] = HDC1080_sensor->temp_C;
	if (sampleAverageIndex > userSettings->totalSamples) {
		sampleAverageIndex = 0;
	}
	float averageTemp_C = 0.0;
	for (int i = 0; i < userSettings->totalSamples; i++) {
		averageTemp_C += temperatureSamples[i];
	}
	averageTemp_C /= userSettings->totalSamples;

	runCycle(averageTemp_C);
};

bool preRunChecklist()
{
	// Check if motion has been detected, if not then don't run the furnace
	if (!motionTimeoutCheck(userSettings->motionDetectorSec))
		return false;
	// TODO check if new cycle needs to be loaded into settings
	// All checks passed
	return true;
};

void furnaceRelay(bool powerON)
{
	bool relayON;
	int result = GPIO_GetValue(furnaceRelayStateFd, &relayON);
	if ((powerON != relayON)) // if the furnace state matches the desired state, don't toggle the relay
	{
		// TODO calculate runtime
		struct timespec currentTime;
		clock_gettime(CLOCK_REALTIME, &currentTime);
		if (powerON) {
			furnaceStartTime = currentTime.tv_sec;
		}
		else {
			furnaceStopTime = currentTime.tv_sec;
			furnaceRunTime = furnaceStopTime - furnaceStartTime;
			Log_Debug("RUNTIME: %d\n", furnaceRunTime);
		}
		//Log_Debug("[INFO:] In furnaceRelay\n");
		//const struct timespec sleepTime = { 0, 50000000 }; // 50 ms
		//GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_Low); // connect opendrain
		//nanosleep(&sleepTime, NULL);
		//GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_High); // go back to Z state
	}
	if (powerON)
		GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_Low);
	else
		GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_High);
};

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