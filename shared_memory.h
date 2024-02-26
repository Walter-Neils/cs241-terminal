#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdio.h>
#include <sys/mman.h>

typedef struct shared_memory {
  void* memory;
  size_t size;
} shared_memory;

void shared_memory_initialize(shared_memory* shared_mem, size_t size);
void* shared_memory_initialize_from_existing_local_memory(shared_memory* shared_mem, void* src, size_t size);
void shared_memory_free(shared_memory* shared_mem);

#endif
