#pragma once

#include <stdbool.h>
#include <time.h>
#include "mt3620.h"
#include "oled.h"
#include "thermostat.h"
#include "msg.h"

bool motionTimeoutCheck(unsigned int timeout);

void motionTimer(unsigned int screenTimeout);