#ifndef INPUT_H
#define INPUT_H
#include <stdio.h>
#include <stdlib.h>

// FIXME: These names are not correctly cased
typedef enum input_state_terminate_flags {
  input_state_terminate_flags_none = 0,
  input_state_terminate_flags_newline = 1 << 0
} input_state_terminate_flags;

// FIXME: These names are not correctly cased
typedef enum input_state_update_status {
  input_state_update_status_none,
  input_state_update_status_resultready,
  input_state_update_status_readagain_async_nodataavailable,
  input_state_update_status_buffer_exhausted,
  input_state_update_status_unknown_error
} input_state_update_status;


typedef struct input_state {
  char* value_buffer;
  size_t value_buffer_size;
  int fd;
  input_state_terminate_flags termination_flags;
} input_state;

void input_state_initialize(input_state* input_state, int fd, size_t buffer_size);
void input_state_allow_nonblocking_reads(input_state* state);
input_state_update_status input_state_update(input_state* state);
void input_state_clear(input_state* state);

#endif
