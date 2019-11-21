#include "schedule.h"

void initCycle(struct thermostatSettings *userSettings_ptr) {
	// TODO remove hardcoded defaults and replace these with ones loaded from current schedule
	userSettings_ptr->targetTemp_F = 70.0;
	userSettings_ptr->lower_threshold = 2.0;
	userSettings_ptr->upper_threshold = 1.0;
	userSettings_ptr->totalSamples = 5;
	userSettings_ptr->samplePeriod.tv_nsec = 0;
	userSettings_ptr->samplePeriod.tv_sec = 30;
	userSettings_ptr->baselineTemp_F = 50.0;
	userSettings_ptr->motionDetectorSec = 43200;
	userSettings_ptr->screenTimeoutSec = 30;
	userSettings_ptr->currentCycle = NULL;
	
	int id = 0;
	for (int i = 0; i < 7; i++) {
		day[i] = malloc(sizeof(cycle_t));
		day[i]->id = id++;
		day[i]->start_hour = 22;
		day[i]->start_min = 0;
		day[i]->temp_F = 72.0;
		day[i]->prev = NULL;

		push_end(day[i], id++, 20, 0, 60.0);
		push_end(day[i], id++, 18, 5, 72.0);
		push_end(day[i], id++, 8, 30, 60.0);
		push_end(day[i], id++, 7, 10, 70.0);
		push_end(day[i], id++, 0, 0, 60.0);
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

		sendCURLStats(userSettings_ptr->targetTemp_F, userSettings_ptr->lower_threshold, userSettings_ptr->upper_threshold, loadedCycle->temp_F, userSettings_ptr->lower_threshold, userSettings_ptr->upper_threshold);

		userSettings_ptr->currentCycle = loadedCycle;
		userSettings_ptr->targetTemp_F = loadedCycle->temp_F;

		Log_Debug(" -===- loaded cycle is: %d:%d (%.1f F°)\n", loadedCycle->start_hour, loadedCycle->start_min, loadedCycle->temp_F);
	}
	return false;
}