#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VECTOR_CRITICAL_ERROR(x) perror("[VECTOR] " x)

void vector_initialize(vector *target, size_t element_size) {
  target->element_size = element_size;
  target->current_used_slots = 0;
  target->current_allocated_slots = 0;
  target->data = NULL;
#if VECTOR_USE_INIT_GUARD
  target->__init_guard_magic = VECTOR_INIT_GUARD_MAGIC;
#endif
}

void vector_ensure_space_for(vector *target, size_t desired_element_count) {
  VECTOR_INIT_GUARD(target);
  void *new_allocation =
      realloc(target->data, desired_element_count * target->element_size);
  if (new_allocation == NULL) {
    VECTOR_CRITICAL_ERROR("Failed to allocate memory for elements");
    exit(-1);
  }
  target->current_allocated_slots = desired_element_count;
  target->data = new_allocation;
}

void vector_push_back(vector *target, void *element) {
  VECTOR_INIT_GUARD(target);
  if (target->current_allocated_slots - target->current_used_slots < 1) {
    vector_ensure_space_for(target, target->current_allocated_slots + 1);
  }
  void *destination =
      vector_get_element_at_index(target, target->current_used_slots);
  memcpy(destination, element, target->element_size);
  target->current_used_slots++;
}

void *vector_get_element_at_index(vector *target, size_t index) {
  VECTOR_INIT_GUARD(target);
  return target->data + (index * target->element_size);
}

void vector_clear(vector *target) {
  VECTOR_INIT_GUARD(target);
  target->current_used_slots = 0;
}

void vector_destroy(vector *target) {
  VECTOR_INIT_GUARD(target);
  if (target->data != NULL) {
    free(target->data);
  }
  target->current_used_slots = 0;
  target->current_allocated_slots = 0;
#if VECTOR_USE_INIT_GUARD
  target->__init_guard_magic = 0;
#endif
}

void vector_remove_at(vector *vector, size_t index) {
  for (size_t i = index + 1; i < vector->current_used_slots; i++) {
    void *destination = vector_get_element_at_index(vector, i - 1);
    void *src = vector_get_element_at_index(vector, i);
    memcpy(destination, src, vector->element_size);
  }
  vector->current_used_slots--;
}

size_t vector_length(vector *target) { return target->current_used_slots; }

void vector_iterator_initialize(vector_iterator *iterator, vector *vector) {
  iterator->current_position = -1;
  iterator->target_vector = vector;
}

bool vector_iterator_move_next(vector_iterator *iterator) {
  iterator->current_position++;
  return iterator->current_position <
             iterator->target_vector->current_used_slots &&
         iterator->current_position >= 0;
}

void *vector_iterator_current_element(vector_iterator *iterator) {
  return vector_get_element_at_index(iterator->target_vector,
                                     iterator->current_position);
}
