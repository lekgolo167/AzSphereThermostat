#include "schedule.h"

int currentCycleDay = -1;
int currentCycleHour = -1;
int currentCycleMin = -1;

void initCycle(struct thermostatSettings *userSettings_ptr) {
	// TODO remove hardcoded defaults and replace these with ones loaded from current schedule
	userSettings_ptr->targetTemp_C = 20.0;
	userSettings_ptr->lower_threshold = 2.0;
	userSettings_ptr->upper_threshold = 1.0;
	userSettings_ptr->totalSamples = 5;
	userSettings_ptr->samplePeriod.tv_nsec = 0;
	userSettings_ptr->samplePeriod.tv_sec = 15;
	userSettings_ptr->baselineTemp_C = 10.0;
	userSettings_ptr->motionDetectorSec = 43200;
	userSettings_ptr->screenTimeoutSec = 30;
	
	for (int i = 0; i < 7; i++) {
		day[i] = malloc(sizeof(cycle_t));
		day[i]->start_hour = 20;
		day[i]->start_min = 0;
		day[i]->temp_C = 24.0;
		float count = 23.0;

		for (int j = 14; j > 2; j -= 1) {
			push_end(day[i], j, 30, count);
			count -= 1.0;
		}
		push_end(day[i], 0, 0, count);
	}

	for (int i = 0; i < 7; i++) {
		Log_Debug("-==- %d\n", i);
		print_list(day[i]);

	}
	
}

bool cycleExpired(struct thermostatSettings *userSettings_ptr) {
	struct timespec currentTime;
	clock_gettime(CLOCK_REALTIME, &currentTime);
	struct tm * now = localtime(&currentTime.tv_sec);
	
	if (currentCycleDay != now->tm_wday) {
		currentCycleDay = now->tm_wday;
		findNextCycle(day[currentCycleDay], now->tm_hour, now->tm_min);
		Log_Debug("DAY IS %d \n", currentCycleDay);
	}
	return false;
}