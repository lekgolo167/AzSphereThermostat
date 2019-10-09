#include "schedule.h"

void initCycle(struct thermostatSettings *userSettings_ptr) {
	// TODO remove hardcoded defaults and replace these with ones loaded from current schedule
	userSettings_ptr->targetTemp_C = 24.0;
	userSettings_ptr->temp_C_Threshold = 2.0;
	userSettings_ptr->totalSamples = 2;
	userSettings_ptr->samplePeriod.tv_nsec = 0;
	userSettings_ptr->samplePeriod.tv_sec = 5;
	userSettings_ptr->baselineTemp_C = 10.0;
}

bool cycleExpired(struct thermostatSettings *userSettings_ptr) {
	return false;
}