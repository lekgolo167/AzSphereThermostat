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

	// All checks passed
	return true;
};

void furnaceRelay(bool powerON)
{
	//bool relayON;
	//int result = GIPO_Get_value(furnaceRelayStateFd, &relayON);
	//if ((powerON != relayON)) // if the furnace state matches the desired state, don't toggle the relay
	//{
	//  Log_Debug("[INFO:] In furnaceRelay\n");
	//  //const struct timespec sleepTime = { 0, 50000000 }; // 50 ms
	//  GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_Low); // connect opendrain
	//  nanosleep(&sleepTime, NULL);
	//  GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_High); // go back to Z state
	//}
	if (powerON)
		GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_Low);
	else
		GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_High);
};