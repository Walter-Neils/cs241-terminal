#ifndef CMDLAYER_H
#define CMDLAYER_H

#include "vector.h"

typedef enum CMD_LAYER_RESULT {
  CMD_LAYER_RESULT_CANNOT_HANDLE,
  CMD_LAYER_RESULT_HANDLED
} CMD_LAYER_RESULT;

typedef struct cmd_layer {
  const char *name;
  CMD_LAYER_RESULT (*handler)(vector *);
} cmd_layer;

void command_layers_initialize();

vector* command_layers_get_underlying_vector();

#endif
