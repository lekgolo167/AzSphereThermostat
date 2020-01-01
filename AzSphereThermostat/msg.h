#pragma once

#include <curl/curl.h>
#include "applibs_versions.h"
#include <applibs/networking.h>
#include <stdbool.h>
#include <applibs/log.h>
#include "parson.h"
#include <stdlib.h>
#include <string.h>
#include "schedule.h"
#include "linkedList.h"

struct string {
	char *ptr;
	size_t len;
};

void init_string(struct string *s);

size_t writedata(void *ptr, size_t size, size_t nmemb, struct string *s);

bool getDayIDs(int* idArr);

void getCycleData(int day, cycle_t * cycles);

void sendCURL(char* URLAndPath, char* dataFieldBuffer);

void sendCURLStats(float bt, float bl, float bh, float at, float al, float ah);

