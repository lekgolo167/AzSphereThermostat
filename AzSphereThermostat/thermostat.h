#pragma once
#include <applibs/i2c.h>
#include <applibs/log.h>
#include <applibs/gpio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "mt3620.h"
#include "HDC1080.h"
#include "HC-SR04.h"
#include "schedule.h"

extern int relayON;

/// <summary>
///     This function saves the current user settings
/// </summary>
struct thermostatSettings
{
	float targetTemp_C;
	float temp_C_Threshold;
	float baselineTemp_C;
	struct timespec samplePeriod;
	unsigned int totalSamples;
};

/// <summary>
///     This initializes the cycle of the furnace
/// </summary>
void initCycle(struct thermostatSettings *userSettings_ptr, struct HDC1080 *HDC1080_sensor_ptr);

/// <summary>
///     This is the cycle of the furnace from one on state to one off state
/// </summary>
void runCycle();

/// <summary>
///     This function waits for the conditions to be right to run the furnace
/// </summary>
void standBy();

/// <summary>
///     This function samples the air and returns an average temp over the course of a set time period
/// </summary>
float sampleTemperature();

/// <summary>
///     This function turns on the furnace relay then samples the air until a threshold is reached
/// </summary>
void runFurnace(float targetTemp_C);

/// <summary>
///     This function checks the pre-requirements to run the furnace
/// </summary>
bool preRunChecklist(bool autoMode);

/// <summary>
///     This function toogles the furnace relay
/// </summary>
void furnaceRelay(bool state);
