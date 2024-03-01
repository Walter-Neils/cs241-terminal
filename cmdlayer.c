#include "cmdlayer.h"

vector command_layers;

void command_layers_initialize() {
  vector_initialize(&command_layers, sizeof(cmd_layer));
}

vector *command_layers_get_underlying_vector() { return &command_layers; }
