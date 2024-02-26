#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include <stdbool.h>
#include <sys/types.h>
#include "vector.h"
#include "shared_memory.h"


typedef struct subprocess {
  pid_t pid;
  bool has_exited;
  int exit_code;
} subprocess;

typedef struct subprocess_create_result {
  shared_memory subprocess_shared_memory_container;
  subprocess* subprocess;
} subprocess_create_result;

string* subprocess_string_vector_to_exec_compatible_memory_region(vector* target);

subprocess_create_result subprocess_run(string filename, vector* argv, vector* environment);
int subprocess_wait_for_exit(subprocess* subprocess);

#endif
