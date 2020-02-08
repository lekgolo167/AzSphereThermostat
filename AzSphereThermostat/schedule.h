#pragma once

#include <stdbool.h>
#include <applibs/rtc.h>
#include <time.h>
#include "msg.h"
#include "thermostat.h"
#include "linkedList.h"

cycle_t * day[7];
int* dayIDs[7];

/// <summary>
///     This checks if the current cycle is outdated 
/// </summary>
bool cycleExpired(struct thermostatSettings *userSettings_ptr);

/// <summary>
///     This initializes the cycle of the furnace
/// </summary>
void initCycle(struct thermostatSettings *userSettings_ptr);

/// <summary>
///     Check the server and see if local schedule IDs match the local IDs, if they don't, download the new schedule
/// </summary>
bool checkServerForScheduleUpdates(struct thermostatSettings *userSettings_ptr);