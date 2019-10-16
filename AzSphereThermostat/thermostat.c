#include "thermostat.h"
#include "oled.h"
struct thermostatSettings *userSettings;
struct HDC1080 *HDC1080_sensor;


void initThermostat(struct thermostatSettings *userSettings_ptr, struct HDC1080 *HDC1080_sensor_ptr)
{
	relayON = false;

	userSettings = userSettings_ptr;

	HDC1080_sensor = HDC1080_sensor_ptr;
};

void runCycle()
{
	Log_Debug("[INFO:] In runCycle\n");
	// Load new cycle and populate settings struct
	initCycle(userSettings);

	do {
		// Stay in standby until room drops below threshold temperature
		float roomTemp_C = standBy();

		// Once room is below threshold, check if the furnace should be on
		if (preRunChecklist(roomTemp_C))
		{
			// Run furnace until room reaches threshold
			runFurnace(userSettings->targetTemp_C);
		}

		// check if schedule expired
	} while(!cycleExpired(userSettings));


};

float standBy()
{
	Log_Debug("[INFO:] In standBy\n");
	float roomTemp_C = 0.0;

	while (1) 
	{
		//oled_draw_logo();
		roomTemp_C = sampleTemperature();
		update_other(roomTemp_C, tempC2F(), 0.0);
		// Check if room is below threshold temperature
		if (roomTemp_C <= (userSettings->targetTemp_C - userSettings->temp_C_Threshold))
		{
			break;
		}
	}
	return roomTemp_C;
};

void runFurnace(float targetTemp_C)
{
	Log_Debug("[INFO:]  runFurnace\n");
	// Turn furnace ON
	furnaceRelay(true);

	while (1)
	{
		float roomTemp_C = sampleTemperature();
		update_other(roomTemp_C, tempC2F(), 0.0);
		// Check if room is above threshold temperature
		if (roomTemp_C >= (targetTemp_C + userSettings->temp_C_Threshold))
		{
			break;
		}
	}

	// Turn furnace OFF
	furnaceRelay(false);
};

float sampleTemperature()
{
	Log_Debug("[INFO:] In sampleTemperature\n");
	float averageTemp_C = 0.0;

	for (unsigned int sample = 0; sample < userSettings->totalSamples; ++sample)
	{
		// wait inbetween samples
		nanosleep(&userSettings->samplePeriod, NULL);
		
		// get the temperature
		if (HDC1080GetTemperature()) {
			averageTemp_C += HDC1080_sensor->temp_C;
			// temporary debug statement
			char c[20];
			sprintf(c, "TEMP -> %f \n", HDC1080_sensor->temp_C);
			Log_Debug(c);
		}
		else // failed to read temp_C try again
		{
			--sample;
		}
		// TODO make this fail if it tries X amount of times
	}

	// temporary debug statement
	char c[20];
	sprintf(c, "AVG -> %f \n", (averageTemp_C / userSettings->totalSamples));
	Log_Debug(c);
	return (averageTemp_C / userSettings->totalSamples);
};

bool preRunChecklist(float roomTemp_C)
{
	Log_Debug("[INFO:] In preRunChecklist\n");
	// If the room temp is below the baseline then turn on the furnace regardless of schedule and motion detection
	if (roomTemp_C <= (userSettings->baselineTemp_C - userSettings->temp_C_Threshold)) {

		runFurnace(userSettings->baselineTemp_C);
		return false;
	}

	// Check if motion has been detected, if not then don't run the furnace
	if (!motion())
		return false;

	// All checks passed
	return true;
};

void furnaceRelay(bool powerON)
{
	Log_Debug("[INFO:] In furnaceRelay\n");
	//const struct timespec sleepTime = { 0, 50000000 }; // 50 ms

	//if ((powerON != relayON)) // if the furnace state matches the desired state, don't toggle the relay
	//{
	//	GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_Low); // connect opendrain
	//	nanosleep(&sleepTime, NULL);
	//	GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_High); // go back to Z state
	//}
	if (powerON)
	GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_High);
	else
		GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_Low);
};