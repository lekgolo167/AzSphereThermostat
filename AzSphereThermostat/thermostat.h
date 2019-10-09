#pragma once
#include <applibs/i2c.h>
#include <applibs/log.h>
#include <applibs/gpio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>

#include "mt3620.h"
#include "HDC1080.h"
#include "HC-SR04.h"
#include "schedule.h"

volatile int relayON;

/// <summary>
///     This struct saves the current user settings
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
///     This function sets up the struct pointers to run the thermostat
/// </summary>
void initThermostat(struct thermostatSettings *userSettings_ptr, struct HDC1080 *HDC1080_sensor_ptr);

/// <summary>
///     Logic to maintain temperature
/// </summary>
void runCycle();

/// <summary>
///     This function waits for the conditions to be right to run the furnace
/// </summary>
float standBy();

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
bool preRunChecklist(float roomTemp_C);

/// <summary>
///     This function toogles the furnace relay
/// </summary>
void furnaceRelay(bool state);
