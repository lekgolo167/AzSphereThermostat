#pragma once

#include <stdbool.h>

#include "thermostat.h"

/// <summary>
///     This checks if the current cycle is outdated 
/// </summary>
bool cycleExpired(struct thermostatSettings *userSettings_ptr);

/// <summary>
///     This initializes the cycle of the furnace
/// </summary>
void initCycle(struct thermostatSettings *userSettings_ptr);