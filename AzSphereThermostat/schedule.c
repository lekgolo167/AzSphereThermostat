#include "schedule.h"

void initCycle(struct thermostatSettings *userSettings_ptr) {
	
	userSettings_ptr->targetTemp_F = 70.0;
	userSettings_ptr->lower_threshold = 2.0;
	userSettings_ptr->upper_threshold = 1.0;
	userSettings_ptr->totalSamples = 5;
	userSettings_ptr->samplePeriod.tv_nsec = 0;
	userSettings_ptr->samplePeriod.tv_sec = 30;
	userSettings_ptr->baselineTemp_F = 55.0;
	userSettings_ptr->motionDetectorSec = 43200;
	userSettings_ptr->screenTimeoutSec = 30;
	userSettings_ptr->currentCycle = NULL;
	
	int id = 0;
	int i = 0;
	// Sunday
	day[i] = malloc(sizeof(cycle_t));
	day[i]->id = id++;
	day[i]->start_hour = 20;
	day[i]->start_min = 0;
	day[i]->temp_F = 72.0;
	day[i]->prev = NULL;

	push_end(day[i], id++, 17, 15, 60.0);
	push_end(day[i], id++, 15, 0, 72.0);
	push_end(day[i], id++, 12, 0, 60.0);
	push_end(day[i], id++, 10, 0, 70.0);
	push_end(day[i], id++, 0, 0, 60.0);

	i++;
	// Monday
	day[i] = malloc(sizeof(cycle_t));
	day[i]->id = id++;
	day[i]->start_hour = 22;
	day[i]->start_min = 30;
	day[i]->temp_F = 70.0;
	day[i]->prev = NULL;

	push_end(day[i], id++, 20, 30, 60.0);
	push_end(day[i], id++, 17, 30, 70.0);
	push_end(day[i], id++, 10, 0, 60.0);
	push_end(day[i], id++, 8, 30, 65.0);
	push_end(day[i], id++, 0, 0, 60.0);

	i++;
	// Tuesday
	day[i] = malloc(sizeof(cycle_t));
	day[i]->id = id++;
	day[i]->start_hour = 20;
	day[i]->start_min = 0;
	day[i]->temp_F = 72.0;
	day[i]->prev = NULL;

	push_end(day[i], id++, 18, 5, 60.0);
	push_end(day[i], id++, 12, 0, 70.0);
	push_end(day[i], id++, 0, 0, 60.0);

	i++;
	// Wednesday
	day[i] = malloc(sizeof(cycle_t));
	day[i]->id = id++;
	day[i]->start_hour = 18;
	day[i]->start_min = 45;
	day[i]->temp_F = 72.0;
	day[i]->prev = NULL;

	push_end(day[i], id++, 8, 30, 60.0);
	push_end(day[i], id++, 7, 10, 70.0);
	push_end(day[i], id++, 0, 0, 60.0);

	i++;
	// Thursday
	day[i] = malloc(sizeof(cycle_t));
	day[i]->id = id++;
	day[i]->start_hour = 20;
	day[i]->start_min = 0;
	day[i]->temp_F = 72.0;
	day[i]->prev = NULL;

	push_end(day[i], id++, 18, 5, 60.0);
	push_end(day[i], id++, 12, 0, 70.0);
	push_end(day[i], id++, 0, 0, 60.0);

	i++;
	// Friday
	day[i] = malloc(sizeof(cycle_t));
	day[i]->id = id++;
	day[i]->start_hour = 22;
	day[i]->start_min = 0;
	day[i]->temp_F = 72.0;
	day[i]->prev = NULL;

	push_end(day[i], id++, 20, 0, 60.0);
	push_end(day[i], id++, 16, 0, 72.0);
	push_end(day[i], id++, 8, 30, 60.0);
	push_end(day[i], id++, 7, 10, 70.0);
	push_end(day[i], id++, 0, 0, 60.0);

	i++;
	// Saturday
	day[i] = malloc(sizeof(cycle_t));
	day[i]->id = id++;
	day[i]->start_hour = 20;
	day[i]->start_min = 0;
	day[i]->temp_F = 72.0;
	day[i]->prev = NULL;

	push_end(day[i], id++, 18, 0, 60.0);
	push_end(day[i], id++, 14, 5, 72.0);
	push_end(day[i], id++, 11, 30, 60.0);
	push_end(day[i], id++, 8, 30, 70.0);
	push_end(day[i], id++, 0, 0, 60.0);

	for (int i = 0; i < 7; i++) {
		Log_Debug("-==- %d\n", i);
		print_list(day[i]);

	}
	userSettings_ptr->currentCycle = day[0];
	cycleExpired(userSettings_ptr);
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