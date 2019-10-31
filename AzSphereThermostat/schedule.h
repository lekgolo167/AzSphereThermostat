#pragma once

#include <stdbool.h>

#include "thermostat.h"


struct cycle {
	int start_hour;
	int start_min;
	int end_hour;
	int end_min;
	float temp_C;
};

struct day {
	struct cycle cycles[5];
};



/// <summary>
///     This checks if the current cycle is outdated 
/// </summary>
bool cycleExpired(struct thermostatSettings *userSettings_ptr);

/// <summary>
///     This initializes the cycle of the furnace
/// </summary>
void initCycle(struct thermostatSettings *userSettings_ptr);