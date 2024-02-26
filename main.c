#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "environment.h"
#include "input.h"
#include "vector.h"
#include "parser.h"
#include "shared_memory.h"
#include <sys/wait.h>
#include "forkhelpers.h"
#include "subprocess.h"
#include "vt100.h"
void print_all_intvect_values(vector* vector) {
  vector_iterator iterator;
  vector_iterator_initialize(&iterator, vector);
  while(vector_iterator_move_next(&iterator)) {
    int* value = vector_iterator_current_element(&iterator);
    printf("%i\n", *value);
  }
}

void print_all_stringvect_values(vector* vector) {
  vector_iterator iterator;
  vector_iterator_initialize(&iterator, vector);
  while(vector_iterator_move_next(&iterator)) {
    string value = *(string*)vector_iterator_current_element(&iterator);
    printf("%s\n", environment_apply_environment_variables(value));
  }
}

char* read_line() {
  char* result = NULL;
  size_t size;
  if(getline(&result, &size, stdin) == -1) {
    return NULL;
  }
  return result;
}

char* move_to_heap(char* src) {
  size_t len = strlen(src);
  char* destination = malloc(len + 1);
  memcpy(destination, src, len + 1);
  return destination;
}

int main() {
  environment_initialize();
  input_state stdin_input_state;
  input_state_initialize(&stdin_input_state, 0, 2048);
  input_state_allow_nonblocking_reads(&stdin_input_state);
  stdin_input_state.termination_flags = input_state_terminate_flags_newline;
  vector argv;
  vector* environment_variables = environment_get_env_vars_vector();
  environment_set_variable("TERM", move_to_heap("WINCMD"));
  vector tokens;
  vector_initialize(&tokens, sizeof(string));
  vector_initialize(&argv, sizeof(string));

  while(true) {
    enum input_state_update_status status = input_state_update(&stdin_input_state);
    if(status == input_state_update_status_resultready) {
      vector_clear(&tokens);
      tokenize_command(stdin_input_state.value_buffer, &tokens);
      print_all_stringvect_values(&tokens);
      input_state_clear(&stdin_input_state);
    } else if(status == input_state_update_status_buffer_exhausted) {
      perror("Buffer exhausted\n");
      exit(EXIT_FAILURE);
    }
  }

  // string targetProgram = "echo";
  // string argument = "/home/walterineils/Documents/source/repos/personal/console/__touch";
  // vector_push_back(&argv, &targetProgram);
  // vector_push_back(&argv, &argument);
  // subprocess_create_result subprocess = subprocess_run("/bin/echo", &argv, environment_variables);
  // int exitcode = subprocess_wait_for_exit(subprocess.subprocess);
  // printf("%i, %i", exitcode, subprocess.subprocess->has_exited);
}
