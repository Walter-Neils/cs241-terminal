#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "vector.h"

void environment_initialize();

vector* environment_get_env_vars_vector();

string environment_get_variable_value(string key);

bool environment_delete_variable(string key);

void environment_set_variable(string key, string value);

void environment_get_available_variables(vector* output);

string environment_raw_key_to_varref_format(string key);

char* environment_apply_environment_variables(char* input);

#endif
