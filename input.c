#include "input.h"
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#ifndef MAX_INPUT_UPDATE_DURATION
#define MAX_INPUT_UPDATE_DURATION 150
#endif

void input_state_initialize(input_state *input_state, int fd,
                            size_t buffer_size) {
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
  tcgetattr(fileno(stdin), &term);
  // turn off Canonical processing in term
  term.c_lflag &= ~ICANON;
  // set the terminal configuration for stdin according to term, now
  tcsetattr(fileno(stdin), TCSANOW, &term);
}

INPUT_STATE_UPDATE_STATUS input_state_update(input_state *state) {
#if USE_ASYNC_IO || true // WHY DOES THIS NEED TO BE TRUE???
  clock_t start = clock();
  do {
    size_t character_insertion_position = strlen(state->value_buffer);
    if (state->value_buffer[0] == 0)
      character_insertion_position = 0;
    char character;
    ssize_t maximum_read_length =
        state->value_buffer_size - character_insertion_position - 1;
    if (maximum_read_length < 1) {
      return INPUT_STATE_UPDATE_STATUS_BUFFER_EXHAUSTED;
    }
    int read_result = read(state->fd, &character, 1);
    if (read_result == -1) {
      // TODO: Add a case for EOF, because currently I'm assuming that the stdin
      // stream never ends
      if (errno == EAGAIN) {
        return INPUT_STATE_UPDATE_STATUS_READAGAIN_ASYNC_NODATAAVAILABLE;
      }
      return INPUT_STATE_UPDATE_STATUS_UNKNOWN_ERROR;
    }

    bool terminate_on_newline_received =
        state->termination_flags & INPUT_STATE_TERMINATE_FLAGS_NEWLINE;
    if (terminate_on_newline_received && character == '\n') {
      return INPUT_STATE_UPDATE_STATUS_RESULT_READY;
    }
    state->value_buffer[character_insertion_position] = character;
  } while (((double)(clock() - start) / CLOCKS_PER_SEC) * 1000 <
           MAX_INPUT_UPDATE_DURATION);
  return INPUT_STATE_UPDATE_STATUS_NONE;
#else
  memset(state->value_buffer, 0, state->value_buffer_size);
  fgets(state->value_buffer, state->value_buffer_size - 1, stdin);
  return INPUT_STATE_UPDATE_STATUS_RESULT_READY;
#endif
}

void input_state_clear(input_state *state) {
  memset(state->value_buffer, 0, state->value_buffer_size);
  for (size_t i = 0; i < state->value_buffer_size; i++) {
    if (state->value_buffer[i] != 0) {
      perror("INPUT BUFFER CONTAINS INCORRECT DATA AFTER CLEAR");
      exit(EXIT_FAILURE);
    }
  }
}
