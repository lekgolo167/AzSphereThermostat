#include "schedule.h"

void initCycle(struct thermostatSettings *userSettings_ptr) {
	// TODO remove hardcoded defaults and replace these with ones loaded from current schedule
	userSettings_ptr->targetTemp_C = 20.0;
	userSettings_ptr->lower_threshold = 2.0;
	userSettings_ptr->upper_threshold = 1.0;
	userSettings_ptr->totalSamples = 5;
	userSettings_ptr->samplePeriod.tv_nsec = 0;
	userSettings_ptr->samplePeriod.tv_sec = 30;
	userSettings_ptr->baselineTemp_C = 10.0;
	userSettings_ptr->motionDetectorSec = 43200;
	userSettings_ptr->screenTimeoutSec = 30;
	userSettings_ptr->currentCycle = NULL;
	
	int id = 0;
	for (int i = 0; i < 7; i++) {
		day[i] = malloc(sizeof(cycle_t));
		day[i]->id = id++;
		day[i]->start_hour = 20;
		day[i]->start_min = 0;
		day[i]->temp_C = 24.0;
		float count = 23.0;

		for (int j = 20; j > 2; j -= 2) {
			push_end(day[i], id++, j, 10, count);
			count -= 1.0;
		}
		push_end(day[i], id++, 0, 0, count);
	}

	for (int i = 0; i < 7; i++) {
		Log_Debug("-==- %d\n", i);
		print_list(day[i]);

	}
	userSettings_ptr->currentCycle = day[0];
}

bool cycleExpired(struct thermostatSettings *userSettings_ptr) {
	struct timespec currentTime;
	clock_gettime(CLOCK_REALTIME, &currentTime);
	struct tm * now = localtime(&currentTime.tv_sec);
	
	cycle_t* loadedCycle = findNextCycle(day[now->tm_wday], now->tm_hour, now->tm_min);
	if (loadedCycle->id != userSettings_ptr->currentCycle->id) {
		userSettings_ptr->currentCycle = loadedCycle;
		userSettings_ptr->targetTemp_C = userSettings_ptr->currentCycle->temp_C;
		Log_Debug(" -===- loaded cycle is: %d:%d (%f C')\n", loadedCycle->start_hour, loadedCycle->start_min, loadedCycle->temp_C);
	}
	return false;
}