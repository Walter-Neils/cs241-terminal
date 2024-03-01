#ifndef BACKGROUND_TASKS_H
#define BACKGROUND_TASKS_H

#include "vector.h"

void background_tasks_initialize();

vector* background_tasks_get_underlying_vector();

size_t background_tasks_prune_and_pop(vector* completed_jobs);

#endif
