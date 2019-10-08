#include "thermostat.h"

volatile int relayON = false;
volatile bool autoMode;

struct thermostatSettings *userSettings;
struct HDC1080 *HDC1080_sensor;


void initCycle(struct thermostatSettings *userSettings_ptr, struct HDC1080 *HDC1080_sensor_ptr)
{
	userSettings = userSettings_ptr;

	// TODO remove hardcoded defaults and replace these with saved defaults from EEPROM
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
	//load new cycle and populate settings struct
	initCycle(userSettings, HDC1080_sensor);
	do {
		// Stay in standby until room drops below threshold temperature
		standBy();

		// Once room is below threshold, check if the furnace should be on
		if (preRunChecklist())
		{
			// Run furnace until room reaches threshold
			runFurnace(userSettings->targetTemp_C);
		}

		// check if schedule expired
	} while();


};

void standBy()
{
	while (1) 
	{
		float roomTemp_C = sampleTemperature();

		// Check if room is below threshold temperature
		if (roomTemp_C <= (userSettings->targetTemp_C - userSettings->temp_C_Threshold))
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
		if (roomTemp_C  >= (targetTemp_C + userSettings->temp_C_Threshold))
		{
			break;
		}
		// If at the end of a schedule cycle, furnace needs to trun off unless temp is below baseline
		if (!checkSchedule() && roomTemp_C >= (userSettings->baselineTemp_C + userSettings->temp_C_Threshold)) break; 
	}

	// Turn furnace OFF
	furnaceRelay(false);
};

bool preRunChecklist(bool autoMode)
{
	// Check manual mode override
	if (!autoMode)
	{
		return false;
	}

	// Check schedule
	if (!check())
	{
		return false;
	}

	// Check motion sensor
	if (!motion())
	{
		return false;
	}

	return true;
};

void furnaceRelay(bool powerON)
{
	const struct timespec sleepTime = { 0, 50000000 }; // 50 ms

	if ((powerON != relayON)) // if the furnace state matches the desired state, don't toggle the relay
	{
		GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_Low); // connect opendrain
		nanosleep(&sleepTime, NULL);
		GPIO_SetValue(GPIO_relay_Fd, GPIO_Value_High); // go back to Z state
	}
};