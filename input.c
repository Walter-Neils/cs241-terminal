#include "input.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <termios.h>

#ifndef MAX_INPUT_UPDATE_DURATION
#define MAX_INPUT_UPDATE_DURATION 150
#endif

void input_state_initialize(input_state *input_state, int fd, size_t buffer_size) {
  input_state->fd = fd;
  input_state->value_buffer = malloc(buffer_size);
  memset(input_state->value_buffer, 0, buffer_size);
  input_state->value_buffer_size = buffer_size;
}

void input_state_allow_nonblocking_reads(input_state *state) { 
  // Combination of two stackoverflow answers
  int flags = fcntl(state->fd, F_GETFL, 0);
  fcntl(state->fd, F_SETFL, flags | O_NONBLOCK);
  // define a terminal configuration data structure
  struct termios term;
  // copy the stdin terminal configuration into term
  tcgetattr( fileno(stdin), &term );
  // turn off Canonical processing in term
  term.c_lflag &= ~ICANON;
  // set the terminal configuration for stdin according to term, now
  tcsetattr( fileno(stdin), TCSANOW, &term);
}

input_state_update_status input_state_update(input_state* state) {
  clock_t start = clock();
  do {
    size_t character_insertion_position = strlen(state->value_buffer);
    if(state->value_buffer[0] == 0) character_insertion_position = 0;
    char character;
    ssize_t maximum_read_length = state->value_buffer_size - character_insertion_position - 1;
    if(maximum_read_length < 1) {
      return input_state_update_status_buffer_exhausted; // We're out of space to read data into
    }
    int read_result = read(state->fd, &character, 1);
    if(read_result == -1) {
      // TODO: Add a case for EOF, because currently I'm assuming that the stdin stream never ends
      if(errno == EAGAIN) {
        return input_state_update_status_readagain_async_nodataavailable;
      }
      return input_state_update_status_unknown_error;
    }

    bool terminate_on_newline_received = state->termination_flags & input_state_terminate_flags_newline;
    if(terminate_on_newline_received && character == '\n') {
      return input_state_update_status_resultready;
    }
    state->value_buffer[character_insertion_position] = character;
  } while(((double)(clock() - start) / CLOCKS_PER_SEC) * 1000 < MAX_INPUT_UPDATE_DURATION);


  return input_state_update_status_none;
}

void input_state_clear(input_state* state) {
  memset(state->value_buffer, 0, state->value_buffer_size);
}
