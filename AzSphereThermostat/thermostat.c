#include "thermostat.h"

struct thermostatSettings *userSettings;
struct HDC1080 *HDC1080_sensor;


void initCycle(struct thermostatSettings *userSettings_ptr, struct HDC1080 *HDC1080_sensor_ptr)
{
	userSettings = userSettings_ptr;

	userSettings->targetTemp_C = 23.0;
	userSettings->temp_C_Threshold = 2.0;
	userSettings->totalSamples = 5;
	userSettings->samplePeriod.tv_nsec = 0;
	userSettings->samplePeriod.tv_sec = 30;

	HDC1080_sensor = HDC1080_sensor_ptr;
	HDC1080Begin(HDC1080_sensor);
};

void runCycle()
{
	// Manual override to either run the furnace or turn it off for one cycle
	bool autoMode = true;

	// Stay in standby until room drops below threshold temperature
	standBy();

	// Once room is below threshold, check if the furnace should be on
	if (preRunChecklist(autoMode))
	{
		// Run furnace until room reaches threshold
		runFurnace(userSettings->targetTemp_C);
	}

};

void standBy()
{
	while (1) 
	{
		float roomTemp_C = sampleTemperature();

		// Check if room is below threshold temperature
		if (userSettings->targetTemp_C - roomTemp_C >= userSettings->temp_C_Threshold)
		{
			break;
		}

		// Maintain room above freezing no matter what the schedule is
		if (roomTemp_C <= (userSettings->baselineTemp_C - userSettings->temp_C_Threshold))
		{
			runFurnace(userSettings->baselineTemp_C);
		}
	}
};

float sampleTemperature()
{
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
			sprintf(c, "TEMP -> %f \n", tempC2F());
			Log_Debug(c);
		}
		else // failed to read temp_C try again
		{
			--sample;
		}
		// TODO make this fail if it tries X amount of times
	}

	return (averageTemp_C / userSettings->totalSamples);
};

void runFurnace(float targetTemp_C)
{
	// Turn furnace ON
	furnaceRelay(true);

	while (1)
	{
		float roomTemp_C = sampleTemperature();

		// Check if room is above threshold temperature
		if (roomTemp_C - targetTemp_C >= userSettings->temp_C_Threshold)
		{
			break;
		}
		// TODO if at the end of a schedule cycle, furnace needs to trun off regardless of what the temp is
	}

	// Turn furnace OFF
	furnaceRelay(false);
};

bool preRunChecklist(bool autoMode)
{
	// Check manual mode override
	if (autoMode)
	{
		return false;
	}

	// Check schedule
	if ()
	{
		return false;
	}

	// Check motion sensor
	if ()
	{
		return false;
	}

	return true;
};

void furnaceRelay(bool state)
{
	// TODO toggle furnace relay
};