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
#include "linkedList.h"


/// <summary>
///     This struct saves the current user settings
/// </summary>
struct thermostatSettings
{
	float targetTemp_F;
	float lower_threshold;
	float upper_threshold;
	float baselineTemp_F;
	float temporaryTarget;
	struct timespec samplePeriod;
	unsigned int totalSamples;
	unsigned int screenTimeoutSec;
	unsigned int motionDetectorSec;
	cycle_t* currentCycle;
};

float averageTemp_F;

/// <summary>
///     This function sets up the struct pointers to run the thermostat
/// </summary>
void initThermostat(struct thermostatSettings *userSettings_p, struct HDC1080 *HDC1080_sensor_p);

/// <summary>
///     Logic to maintain temperature
/// </summary>
void runCycle(float roomTemp_F);

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
void runFurnace(float targetTemp_F);

/// <summary>
///     This function checks the pre-requirements to run the furnace
/// </summary>
bool preRunChecklist();

/// <summary>
///     This function toogles the furnace relay
/// </summary>
void furnaceRelay(bool state);
