#include "subprocess.h"
#include "shared_memory.h"
#include "forkhelpers.h"
#include "vector.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

string* subprocess_string_vector_to_exec_compatible_memory_region(vector* target) {
  size_t required_region_size = (target->element_size * (target->current_used_slots + 1)); // We need an additional slot for the null terminator
  string* region = malloc(required_region_size);
  region[target->current_used_slots] = NULL; // Set null terminator
  vector_iterator iterator;
  vector_iterator_initialize(&iterator, target);
  for(size_t i = 0; vector_iterator_move_next(&iterator); i++) {
    string value = *(string*)vector_iterator_current_element(&iterator);
    region[i] = value;
  }

  if(region[target->current_used_slots] != NULL) {
    perror("subprocess_string_vector_to_exec_compatible_memory_region: null terminator was overwritten");
    exit(EXIT_FAILURE);
  }

  return region;
}

subprocess_create_result subprocess_run(string filename, vector* argv, vector* environment) {
  subprocess_create_result result;
  shared_memory_initialize(&result.subprocess_shared_memory_container, sizeof(typeof(*result.subprocess)));
  result.subprocess = result.subprocess_shared_memory_container.memory;

  pid_t forkval = fork();

  if(FORK_IS_PARENT(forkval)) {
    // Not really anything for us to do here
  } else if(FORK_IS_CHILD(forkval)) {
    string* argv_region = subprocess_string_vector_to_exec_compatible_memory_region(argv);
    string* environment_region = subprocess_string_vector_to_exec_compatible_memory_region(environment);
    
    pid_t subforkval = fork();
    if(FORK_IS_PARENT(subforkval)) {
      result.subprocess->pid = subforkval;
      waitpid(subforkval, &result.subprocess->exit_code, 0);
      result.subprocess->has_exited = true;
    } else if(FORK_IS_CHILD(subforkval)) {
      execve(filename, argv_region, environment_region); // Does not return, overwrites the TEXT, DATA, BSS, and STACK of our current process
      perror("execve failed\n");
      exit(EXIT_FAILURE);
    } else {
      perror("fork failed");
      exit(EXIT_FAILURE);
    }
    free(argv_region);
    free(environment_region);
    exit(EXIT_SUCCESS);
  } else if(FORK_FAILED(forkval)) {
    perror("fork() failed\n");
    exit(EXIT_FAILURE);
  } else {
    perror("You've REALLY messed up some fork logic somewhere. Good luck!");
    exit(EXIT_FAILURE);
  }
  return result;
}

int subprocess_wait_for_exit(subprocess *subprocess) {
  int exitcode;
  waitpid(subprocess->pid, &exitcode, 0);
  return exitcode;
}
