#pragma once
#include <applibs/log.h>
#include <stdlib.h>

typedef struct cycle {
	int id;
	int start_hour;
	int start_min;
	float temp_F;
	struct cycle * next;
	struct cycle * prev;
} cycle_t;

void print_list(cycle_t * head);

cycle_t* findNextCycle(cycle_t * head, int hour, int min);

void push_end(cycle_t * head, int id, int hr, int min, float f);

void push_start(cycle_t ** head, int hr, int min, float f);

void pop(cycle_t ** head);

void remove_last(cycle_t * head);

void remove_by_index(cycle_t ** head, int n);