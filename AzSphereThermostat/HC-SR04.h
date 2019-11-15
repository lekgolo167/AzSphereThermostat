#pragma once

#include <stdbool.h>
#include <time.h>
#include "applibs_versions.h"
#include <applibs/networking.h>
#include <curl/curl.h>
#include "mt3620.h"
#include "oled.h"
#include "thermostat.h"

bool motionTimeoutCheck(unsigned int timeout);

void motionTimer(unsigned int screenTimeout);

static void sendCURL(char* URLAndPath, char* dataFieldBuffer);