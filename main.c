#include "cmdlayer.h"
#include "environment.h"
#include "forkhelpers.h"
#include "input.h"
#include "parser.h"
#include "shared_memory.h"
#include "subprocess.h"
#include "vector.h"
#include "vt100.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

bool file_exists(char *filename) {
  // https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
  // Thanks codebunny
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}

bool are_strings_identical(const char *left, const char *right) {
  return strcmp(left, right) == 0; // DRY or something (laziness)
}

char *move_to_heap(const char *src) {
  // idk what I was doing, and frankly I'm too scared to find out
  size_t len = strlen(src);
  char *destination = malloc(len + 1);
  memcpy(destination, src, len + 1);
  return destination;
}

CMD_LAYER_RESULT clear_handler(vector *parameters) {
  string command = *(string *)vector_get_element_at_index(parameters, 0);
  if (are_strings_identical(command, "clear")) {
    printf("\033[2J\033[H");
    fflush(stdout);
    return CMD_LAYER_RESULT_HANDLED;
  }
  return CMD_LAYER_RESULT_CANNOT_HANDLE;
}
CMD_LAYER_RESULT exit_handler(vector *parameters) {
  if (strcmp("exit", *(string *)vector_get_element_at_index(parameters, 0)) ==
      0) {
    exit(EXIT_SUCCESS);
    return CMD_LAYER_RESULT_HANDLED;
  } else {
    return CMD_LAYER_RESULT_CANNOT_HANDLE;
  }
}
const size_t MAX_PATH = 256; // Pulled this default outta thin air, feel free to
                             // change if it sucks or something
CMD_LAYER_RESULT subprocess_invoke_local_handler(vector *parameters) {
  char resolved_path[MAX_PATH];
  char *unparsed_executable_path =
      *(string *)vector_get_element_at_index(parameters, 0);

  if (unparsed_executable_path[0] != '.')
    return CMD_LAYER_RESULT_CANNOT_HANDLE;

  realpath(unparsed_executable_path, resolved_path);
  if (!file_exists(resolved_path))
    return CMD_LAYER_RESULT_CANNOT_HANDLE;
  subprocess_create_result subprocess_creation_result = subprocess_run(
      resolved_path, parameters, environment_get_env_vars_vector());
  subprocess_wait_for_exit(subprocess_creation_result.subprocess);
  return CMD_LAYER_RESULT_HANDLED;
}
CMD_LAYER_RESULT subprocess_invoke_path_handler(vector *parameters) {
  char resolved_path[MAX_PATH];
  memset(resolved_path, 0, sizeof(resolved_path));
  char *unparsed_executable_path =
      *(string *)vector_get_element_at_index(parameters, 0);

  snprintf(resolved_path, sizeof(resolved_path), "/bin/%s",
           unparsed_executable_path);

  if (!file_exists(resolved_path)) {
    return CMD_LAYER_RESULT_CANNOT_HANDLE;
  }

  subprocess_create_result subprocess_creation_result = subprocess_run(
      resolved_path, parameters, environment_get_env_vars_vector());
  subprocess_wait_for_exit(subprocess_creation_result.subprocess);
  // shared_memory_free(&subprocess_creation_result.subprocess_shared_memory_container); // TODO: This needs to work, because right now we leak memory on every command run
  return CMD_LAYER_RESULT_HANDLED;
}

void create_command_handler_layers() {
  cmd_layer layer;
  layer.name = NULL;
  // Yeah this is cursed, but it's more efficient then recreating the struct
  // over and over again These really don't need to be in any particular order
  // because things have been written correctly
  layer.handler = &clear_handler;
  vector_push_back(command_layers_get_underlying_vector(),
                   &layer); // Don't worry, vector_push_back creates a copy
  layer.handler = &exit_handler;
  vector_push_back(command_layers_get_underlying_vector(), &layer);
  layer.handler = &subprocess_invoke_local_handler;
  vector_push_back(command_layers_get_underlying_vector(), &layer);
  layer.handler = &subprocess_invoke_path_handler;
  vector_push_back(command_layers_get_underlying_vector(), &layer);
}

int main() {
  command_layers_initialize();
  create_command_handler_layers();
  environment_initialize();
  input_state stdin_input_state;
  input_state_initialize(&stdin_input_state, STDIN_FILENO, 2048);
  input_state_allow_nonblocking_reads(&stdin_input_state);
  stdin_input_state.termination_flags = INPUT_STATE_TERMINATE_FLAGS_NEWLINE;
  vector argv;
  vector *environment_variables = environment_get_env_vars_vector();
  environment_set_variable("TERM", move_to_heap("WINCMD")); // Branding :)
  vector tokens;
  vector_initialize(&tokens, sizeof(string));
  vector_initialize(&argv, sizeof(string));

  while (true) {
    enum INPUT_STATE_UPDATE_STATUS status = input_state_update(
        &stdin_input_state); // Pull in whatever data we can from stdin
    if (status == INPUT_STATE_UPDATE_STATUS_RESULT_READY) {
      vector_clear(&tokens);
      tokenize_command(stdin_input_state.value_buffer, &tokens);
      bool executed =
          false; // Flag to determine whether or not we need to yell at the user
      vector_iterator iterator;
      vector_iterator_initialize(&iterator,
                                 command_layers_get_underlying_vector());
      while (vector_iterator_move_next(&iterator)) {
        cmd_layer *layer = vector_iterator_current_element(&iterator);
        CMD_LAYER_RESULT result = layer->handler(&tokens);
        if (result == CMD_LAYER_RESULT_HANDLED) {
          executed = true;
          fflush(stdout); // Unless you like being blind
          break;          // We're done
        }
      }
      if (!executed) {
        printf("ERROR: Could not execute command '%s'\n",
               stdin_input_state.value_buffer); // User incompetence statement
      }

      input_state_clear(&stdin_input_state); // Clear the input state so we can
                                             // pull a new line in
    } else if (status == INPUT_STATE_UPDATE_STATUS_BUFFER_EXHAUSTED) {
      perror("Buffer exhausted\n"); // New plan: give up
      exit(EXIT_FAILURE);
    }
  }
}
