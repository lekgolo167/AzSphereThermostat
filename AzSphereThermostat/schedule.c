#include "schedule.h"

void initCycle(struct thermostatSettings *userSettings_ptr) {
	// TODO remove hardcoded defaults and replace these with ones loaded from current schedule
	userSettings_ptr->targetTemp_C = 21.0;
	userSettings_ptr->lower_threshold = 2.0;
	userSettings_ptr->upper_threshold = 1.0;
	userSettings_ptr->totalSamples = 5;
	userSettings_ptr->samplePeriod.tv_nsec = 0;
	userSettings_ptr->samplePeriod.tv_sec = 15;
	userSettings_ptr->baselineTemp_C = 10.0;
	userSettings_ptr->motionDetectorSec = 44000;
	userSettings_ptr->screenTimeoutSec = 30;
}

bool cycleExpired(struct thermostatSettings *userSettings_ptr) {
	return false;
}