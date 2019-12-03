#pragma once

#include <curl/curl.h>
#include "applibs_versions.h"
#include <applibs/networking.h>
#include <stdbool.h>
#include <applibs/log.h>

void sendCURL(char* URLAndPath, char* dataFieldBuffer);

void sendCURLStats(float bt, float bl, float bh, float at, float al, float ah);