#include "background_tasks.h"
#include "subprocess.h"
#include "vector.h"
#include <stdbool.h>

vector background_tasks_vector;

void background_tasks_initialize() {
  vector_initialize(&background_tasks_vector, sizeof(subprocess_create_result));
}

vector *background_tasks_get_underlying_vector() {
  return &background_tasks_vector;
}

size_t background_tasks_prune_and_pop(vector *completed_jobs) {
  size_t count = 0;
  while (true) {
    vector_iterator iterator;
    vector_iterator_initialize(&iterator,
                               background_tasks_get_underlying_vector());
    bool removal_was_required = false;
    while (vector_iterator_move_next(&iterator)) {
      subprocess_create_result *item =
          vector_iterator_current_element(&iterator);
      if (item->subprocess->has_exited) {
        removal_was_required = true;
        count++;
        if (completed_jobs != NULL)
          vector_push_back(completed_jobs, item);
        vector_remove_at(&background_tasks_vector, iterator.current_position);
      }
    }

    if (!removal_was_required) {
      break;
    }
  }
  return count;
}
