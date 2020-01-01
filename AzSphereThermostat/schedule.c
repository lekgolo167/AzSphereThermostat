#include "schedule.h"

void initCycle(struct thermostatSettings *userSettings_ptr) {
	
	userSettings_ptr->targetTemp_F = 70.0;
	userSettings_ptr->lower_threshold = 2.0;
	userSettings_ptr->upper_threshold = 1.0;
	userSettings_ptr->totalSamples = 10;
	userSettings_ptr->samplePeriod.tv_nsec = 0;
	userSettings_ptr->samplePeriod.tv_sec = 30;
	userSettings_ptr->baselineTemp_F = 58.0;
	userSettings_ptr->motionDetectorSec = 43200;
	userSettings_ptr->screenTimeoutSec = 15;
	userSettings_ptr->currentCycle = NULL;
	
	for (int i = 0; i < 7; i++) {
		dayIDs[i] = 0;
		day[i] = NULL;
	}

	bool serverRunning = checkServerForScheduleUpdates();
	if (!serverRunning) {

		int id = 0;
		for (int i = 0; i < 7; i++) {
			day[i] = malloc(sizeof(cycle_t));
			day[i]->id = id++;
			day[i]->start_hour = 12;
			day[i]->start_min = 0;
			day[i]->temp_F = 70.0;
			day[i]->prev = NULL;

			push_end(day[i], id++, 0, 0, 60.0);
		}
	}
	Log_Debug("Server status: %d\n", serverRunning);
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

bool checkServerForScheduleUpdates() {
	Log_Debug("Checking for updated Day IDs\n");
	int* serverIDs[7];
	if (getDayIDs(serverIDs)) {
		for (int i = 0; i < 7; i++) {
			if (serverIDs[i] != dayIDs[i]) {
				Log_Debug("Getting day:%d\n\tServer ID: %d, Local ID: %d", i, serverIDs[i], dayIDs[i]);

				while (remove_last(day[i])>0);
				day[i] = malloc(sizeof(cycle_t));
				day[i]->id = -1;
				day[i]->next = NULL;
				day[i]->prev = NULL;
				getCycleData(i, day[i]);

				dayIDs[i] = serverIDs[i];
				print_list(day[i]);
			}
		}
		return true;
	}
	else {
		return false;
	}
};
