#include "linkedList.h"

void print_list(cycle_t * head) {
	cycle_t * current = head;

	while (current != NULL) {
		Log_Debug("(%d) %d:%d (%.1f F°)\n", current->id, current->start_hour, current->start_min, current->temp_F);
		current = current->next;
	}
}

cycle_t* findNextCycle(cycle_t * head, int hour, int min) {
	cycle_t * current = head;
	while (current != NULL) {
		if (current->start_hour > hour)
			current = current->next;
		else if (current->start_hour == hour && current->start_min > min)
			current = current->next;
		else {
			return current;
		}
	}
	return current;
}

// add item to the end of the list
void push_end(cycle_t * head, int id, int hr, int min, float f) {
	
	if (head->id != -1) {
		cycle_t * current = head;
		while (current->next != NULL) {
			current = current->next;
		}
		/* now we can add a new variable */
		current->next = malloc(sizeof(cycle_t));
		current->next->id = id;
		current->next->start_hour = hr;
		current->next->start_min = min;
		current->next->temp_F = f;
		current->next->next = NULL;
		current->next->prev = current;
	}
	else {
		head->id = id;
		head->start_hour = hr;
		head->start_min = min;
		head->temp_F = f;
		head->next = NULL;
		head->prev = NULL;
	}
}

// add item to the begining of the list
void push_start(cycle_t ** head, int hr, int min, float f) {
	cycle_t * new_node;
	new_node = malloc(sizeof(cycle_t));

	new_node->next->start_hour = hr;
	new_node->next->start_min = min;
	new_node->next->temp_F = f;
	new_node->next = *head;
	*head = new_node;
}

//remove first item
void pop(cycle_t ** head) {
	int retval = -1;
	cycle_t * next_node = NULL;

	if (*head == NULL) {
		return -1;
	}

	next_node = (*head)->next;
	free(*head);
	*head = next_node;

	return;
}

// remove last item
int remove_last(cycle_t * head) {
	if (head == NULL)
		return 0;
	/* if there is only one item in the list, remove it */
	if (head->next == NULL) {
		free(head);
		head = NULL;
		return 0;
	}

	/* get to the second to last node in the list */
	cycle_t * current = head;
	while (current->next->next != NULL) {
		current = current->next;
	}

	/* now current points to the second to last item of the list, so let's remove current->next */
	free(current->next);
	current->next = NULL;
	return 1;

}

// remove item at index
void remove_by_index(cycle_t ** head, int n) {
	int i = 0;
	int retval = -1;
	cycle_t * current = *head;
	cycle_t * temp_node = NULL;

	if (n == 0) {
		pop(head);
		return;
	}

	for (i = 0; i < n - 1; i++) {
		if (current->next == NULL) {
			return -1;
		}
		current = current->next;
	}

	temp_node = current->next;
	current->next = temp_node->next;
	free(temp_node);

	return;

}