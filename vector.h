#ifndef VECTOR_H
#define VECTOR_H


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef char* string;

#ifndef VECTOR_USE_INIT_GUARD
#define VECTOR_USE_INIT_GUARD 0
#endif


#if VECTOR_USE_INIT_GUARD
#define VECTOR_INIT_GUARD_MAGIC ((size_t)0x231290);
#define VECTOR_INIT_GUARD(x) do {if(x->__init_guard_magic != VECTOR_USE_INIT_GUARD) {perror("attempted to use uninitialized vector"); exit(-1);}} while(false); 
#else 
#define VECTOR_INIT_GUARD(x)
#endif



typedef struct vector {
  size_t element_size;
  size_t current_allocated_slots;
  size_t current_used_slots;
  uint8_t* data;
#if VECTOR_USE_INIT_GUARD
  size_t __init_guard_magic;
#endif
} vector;


void vector_initialize(vector* target, size_t element_size);
void vector_ensure_space_for(vector* target, size_t desired_element_count);
void vector_push_back(vector* target, void* element);
void* vector_get_element_at_index(vector* target, size_t index);
void vector_clear(vector* target);
void vector_destroy(vector* target);
void vector_remove_at(vector* vector, size_t index);
size_t vector_length(vector* vector);

typedef struct vector_iterator {
  vector* target_vector;
  ssize_t current_position;
} vector_iterator;

void vector_iterator_initialize(vector_iterator* iterator, vector* vector);
bool vector_iterator_move_next(vector_iterator* iterator);
void* vector_iterator_current_element(vector_iterator* iterator);

#endif
