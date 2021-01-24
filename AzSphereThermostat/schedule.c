#include "schedule.h"

void initCycle(struct thermostatSettings *userSettings_ptr) {
	
	// Set defaults on startup
	userSettings_ptr->targetTemp_F = 68.0;
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

	bool serverRunning = checkServerForScheduleUpdates(userSettings_ptr);

	if (!serverRunning) {

		// If no connection to the server then just have a defalut schedule loaded in
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
		// Start the current cycle pointer to a known value the check for the current cycle and update this pointer
		userSettings_ptr->currentCycle = day[0];
		cycleExpired(userSettings_ptr);
	}
	Log_Debug("Server status: %d\n", serverRunning);
	for (int i = 0; i < 7; i++) {
		Log_Debug("-==- %d\n", i);
		// Show the schedule
		print_list(day[i]);
	}
	
}

bool cycleExpired(struct thermostatSettings *userSettings_ptr) {
	// Get the current time
	struct timespec currentTime;
	clock_gettime(CLOCK_REALTIME, &currentTime);
	struct tm * now = localtime(&currentTime.tv_sec);
	
	// Find which cycle we should be on
	cycle_t* loadedCycle = findNextCycle(day[now->tm_wday], now->tm_hour, now->tm_min);
	// If the IDs do not match then the current cycle has expired and a new cycle needs to be loaded in
	if (loadedCycle->id != userSettings_ptr->currentCycle->id) {
		
		// Update pointers
		userSettings_ptr->currentCycle = loadedCycle;
		userSettings_ptr->targetTemp_F = loadedCycle->temp_F;

		// Sends data to plot the schedule
		sprintf(CURLMessageBuffer, "TARGET=%f&THRESH_L=%f&THRESH_H=%f\0", userSettings_ptr->targetTemp_F, userSettings_ptr->lower_threshold, userSettings_ptr->upper_threshold);
		sendCURL(URL_STATS, CURLMessageBuffer);

		Log_Debug(" -===- loaded cycle is: %d:%d (%.1f F�)\n", loadedCycle->start_hour, loadedCycle->start_min, loadedCycle->temp_F);
	}
	return false;
}

bool checkServerForScheduleUpdates(struct thermostatSettings *userSettings_ptr) {
	Log_Debug("Checking for updated Day IDs\n");
	// Temporary store for the servers IDs
	int* serverIDs[7];
	struct timespec currentTime;
	clock_gettime(CLOCK_REALTIME, &currentTime);
	struct tm* now = localtime(&currentTime.tv_sec);
	if (CURL_enabled && getDayIDs(serverIDs)) { // If server responded
		for (int i = 0; i < 7; i++) {
			if (serverIDs[i] != dayIDs[i]) { // Compare IDs one by one, if they dont match delete the current day's scheudle and load in a new one
				Log_Debug("Getting day:%d\n\tServer ID: %d, Local ID: %d", i, serverIDs[i], dayIDs[i]);

				// Delete one by one each cycle in the day linked list
				while (remove_last(day[i])>0);
				day[i] = NULL;
				day[i] = malloc(sizeof(cycle_t));
				day[i]->id = -1;
				day[i]->next = NULL;
				day[i]->prev = NULL;
				// Ask the server for the schedule for that specific day
				getCycleData(i, day[i]);

				// Update the local copy of the server IDs
				dayIDs[i] = serverIDs[i];
				print_list(day[i]);
			
				// If the day schedule that we happened to update was the cycle currently running then need to update null pointers that were created by freeing memory
				if (now->tm_wday == i) {
					Log_Debug("UPDATING CURRENT DAY");
					

					cycle_t* loadedCycle = findNextCycle(day[now->tm_wday], now->tm_hour, now->tm_min);
					if (loadedCycle != NULL) {
						// Update pointers
						userSettings_ptr->currentCycle = loadedCycle;
						userSettings_ptr->targetTemp_F = loadedCycle->temp_F;
					}

					// Sends data to plot the schedule
					sprintf(CURLMessageBuffer, "TARGET=%f&THRESH_L=%f&THRESH_H=%f\0", userSettings_ptr->targetTemp_F, userSettings_ptr->lower_threshold, userSettings_ptr->upper_threshold);
					sendCURL(URL_STATS, CURLMessageBuffer);
				}
			}
		}
		return true;
	}
	else {
		return false;
	}
};
