#include "thermostat.h"
#include "oled.h"

struct thermostatSettings *userSettings;
struct HDC1080 *HDC1080_sensor;

float temperatureSamples[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int sampleAverageIndex = 0;

void initThermostat(struct thermostatSettings *userSettings_ptr, struct HDC1080 *HDC1080_sensor_ptr)
{
	userSettings = userSettings_ptr;

	HDC1080_sensor = HDC1080_sensor_ptr;
	const struct timespec sleeptime = { 0, 50000000 };
	while (!HDC1080GetTemperature()) {
		Log_Debug("failed to get temperature\n");
		nanosleep(&sleeptime, NULL);
	};

	Log_Debug("initializing sample arrary with: %f\n", HDC1080_sensor_ptr->temp_F);
	for (int i = 0; i < 20; i++) {
		temperatureSamples[i] = HDC1080_sensor_ptr->temp_F;
	}
	averageTemp_F = temperatureSamples[0];
};

void runCycle(float roomTemp_F)
{
	int8_t state = -1;
	bool checklist = preRunChecklist();

	// Check if room is below target temperature
	if (checklist && roomTemp_F <= (userSettings->targetTemp_F - userSettings->lower_threshold))
	{
		Log_Debug("[INFO:] Below target\n");
		// Run furnace until room reaches target
		state = true;
	}
	// Check if room is above target temperature
	else if (checklist && roomTemp_F >= (userSettings->targetTemp_F + userSettings->upper_threshold))
	{
		Log_Debug("[INFO:] Above target\n");
		state = false;
	}
	// Check if room is below baseline temperature
	else if (roomTemp_F <= (userSettings->baselineTemp_F - userSettings->lower_threshold)) {
		// Run furnace until room reaches baseline
		Log_Debug("[INFO:] Below baseline\n");
		state = true;
	}
	// check if room is above baseline temperature
	else if (!checklist && roomTemp_F >= (userSettings->baselineTemp_F + userSettings->upper_threshold))
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

	temperatureSamples[sampleAverageIndex++] = HDC1080_sensor->temp_F;
	if (sampleAverageIndex > userSettings->totalSamples) {
		sampleAverageIndex = 0;
	}
	averageTemp_F = 0.0;
	for (int i = 0; i < userSettings->totalSamples; i++) {
		averageTemp_F += temperatureSamples[i];
	}
	averageTemp_F /= userSettings->totalSamples;

	runCycle(averageTemp_F);
};

bool preRunChecklist()
{
	// check if new cycle needs to be loaded into settings
	cycleExpired(userSettings);
	
	// Check if motion has been detected, if not then don't run the furnace
	if (!motionTimeoutCheck(userSettings->motionDetectorSec))
		return false;
	// All checks passed
	return true;
};

void furnaceRelay(bool powerON)
{
	bool relayON;
	int result = GPIO_GetValue(furnaceRelayStateFd, &relayON);
	Log_Debug("RELAY STATE %d, POWER %d\n", relayON, powerON);
	if ((powerON != relayON)) // if the furnace state matches the desired state, don't toggle the relay
	{
		struct timespec currentTime;
		clock_gettime(CLOCK_REALTIME, &currentTime);
		if (powerON) {
			furnaceStartTime = currentTime.tv_sec;
		}
		else {
			furnaceRunTime = currentTime.tv_sec - furnaceStartTime;
			Log_Debug("RUNTIME: %d\n", furnaceRunTime);

			sprintf(CURLMessageBuffer, "RUNTIME=%d\0", furnaceRunTime);
			sendCURL(URL_RUNTIME, CURLMessageBuffer);
		}
		Log_Debug("[INFO:] In furnaceRelay\n");
		const struct timespec sleepTime = { 0, 50000000 }; // 50 ms
		GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_High);
		nanosleep(&sleepTime, NULL);
		GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_Low);

		sprintf(CURLMessageBuffer, "F_State=%d\0", relayON);
		sendCURL(URL_FURNANCE_STATE, CURLMessageBuffer);

		nanosleep(&sleepTime, NULL);

		sprintf(CURLMessageBuffer, "F_State=%d\0", powerON);
		sendCURL(URL_FURNANCE_STATE, CURLMessageBuffer);
	}
};