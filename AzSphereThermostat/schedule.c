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
	userSettings_ptr->motionDetectorSec = 43200;
	userSettings_ptr->screenTimeoutSec = 30;
	const struct day schedule[] =
	{
		{
			.cycles = {
		//Sunday
			{
				.start_hour = 0,
				.start_min = 0,
				.end_hour = 6,
				.end_min = 0,
				.temp_C = 15.0
			},{
				.start_hour = 6,
				.start_min = 1,
				.end_hour = 8,
				.end_min = 0,
				.temp_C = 22.0
			},
			{
				.start_hour = 8,
				.start_min = 1,
				.end_hour = 18,
				.end_min = 0,
				.temp_C = 15.0
			},
			{
				.start_hour = 18,
				.start_min = 1,
				.end_hour = 23,
				.end_min = 59,
				.temp_C = 22.0
			}
		}
	},
	{
		.cycles = {
		//Monday
		{
			.start_hour = 0,
			.start_min = 0,
			.end_hour = 6,
			.end_min = 0,
			.temp_C = 15.0
		},
		{
			.start_hour = 6,
			.start_min = 1,
			.end_hour = 8,
			.end_min = 0,
			.temp_C = 22.0
		},
		{
			.start_hour = 8,
			.start_min = 1,
			.end_hour = 18,
			.end_min = 0,
			.temp_C = 15.0
		},
		{
			.start_hour = 18,
			.start_min = 1,
			.end_hour = 23,
			.end_min = 59,
			.temp_C = 22.0
		}
	}
},
	};
	float test = schedule[0].cycles[1].temp_C;
	Log_Debug("TESTSSSSSSSS   %f\n", test);
}

bool cycleExpired(struct thermostatSettings *userSettings_ptr) {
	return false;
}