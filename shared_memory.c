#include "shared_memory.h"
#include <string.h>
#include <unistd.h>

void shared_memory_initialize(shared_memory *shared_mem, size_t size) {
  shared_mem->memory = mmap(NULL, size, PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  shared_mem->size = size;
}

void *
shared_memory_initialize_from_existing_local_memory(shared_memory *shared_mem,
                                                    void *src, size_t size) {
  shared_mem->memory = mmap(NULL, size, PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  shared_mem->size = size;
  memcpy(shared_mem->memory, src, size);
  return shared_mem->memory;
}

void shared_memory_free(shared_memory *shared_mem) {
  if (shared_mem->memory != NULL) {
    munmap(shared_mem->memory, shared_mem->size);
    shared_mem->memory = NULL;
  }
}
