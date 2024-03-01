#ifndef INPUT_H
#define INPUT_H
#include <stdio.h>
#include <stdlib.h>

// FIXME: These names are not correctly cased
typedef enum INPUT_STATE_TERMINATE_FLAGS {
  INPUT_STATE_TERMINATE_FLAGS_NONE = 0,
  INPUT_STATE_TERMINATE_FLAGS_NEWLINE = 1 << 0,
} INPUT_STATE_TERMINATE_FLAGS;

// FIXME: These names are not correctly cased
typedef enum INPUT_STATE_UPDATE_STATUS {
  INPUT_STATE_UPDATE_STATUS_NONE,
  INPUT_STATE_UPDATE_STATUS_RESULT_READY,
  INPUT_STATE_UPDATE_STATUS_READAGAIN_ASYNC_NODATAAVAILABLE,
  INPUT_STATE_UPDATE_STATUS_BUFFER_EXHAUSTED,
  INPUT_STATE_UPDATE_STATUS_UNKNOWN_ERROR,
} INPUT_STATE_UPDATE_STATUS;

typedef struct input_state {
  char *value_buffer;
  size_t value_buffer_size;
  int fd;
  INPUT_STATE_TERMINATE_FLAGS termination_flags;
} input_state;

void input_state_initialize(input_state *input_state, int fd,
                            size_t buffer_size);
void input_state_allow_nonblocking_reads(input_state *state);
INPUT_STATE_UPDATE_STATUS input_state_update(input_state *state);
void input_state_clear(input_state *state);

#endif
