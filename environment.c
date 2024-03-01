#include "environment.h"
#include "stringhelpers.h"
#include "vector.h"
#include <stdlib.h>
#include <string.h>

vector environment_variables;

void environment_initialize() {
  vector_initialize(&environment_variables, sizeof(string));
}

vector *environment_get_env_vars_vector() { return &environment_variables; }

bool is_key_match(string key, string target) {
  size_t key_len = strlen(key);
  size_t target_len = strlen(target);
  if (target_len <= key_len + 1)
    return false; // Key length plus 1 for the equal sign
  if (strncmp(key, target, key_len) == 0) {
    // Yeah we found it
    return true;
  } else {
    return false;
  }
}

string environment_get_variable_value(string key) {
  vector_iterator iterator;
  vector_iterator_initialize(&iterator, &environment_variables);
  while (vector_iterator_move_next(&iterator)) {
    string target = *(string *)vector_iterator_current_element(&iterator);
    if (is_key_match(key, target)) {
      size_t key_length = strlen(key);
      return target + key_length +
             1; // Length of key + 1 (length of equal sign)
    }
  }
  return NULL;
}

bool environment_delete_variable(string key) {
  vector_iterator iterator;
  vector_iterator_initialize(&iterator, &environment_variables);
  while (vector_iterator_move_next(&iterator)) {
    string target = *(string *)vector_iterator_current_element(&iterator);
    if (is_key_match(key, target)) {
      vector_remove_at(&environment_variables, iterator.current_position);
      return true;
    }
  }
  return false;
}

void environment_set_variable(string key, string value) {
  environment_delete_variable(key); // Remove it if it already exists
  size_t key_length = strlen(key);
  size_t value_length = strlen(value);
  size_t result_length = sizeof(char) * (key_length + value_length + 1 + 1);
  string key_value_pair =
      malloc(result_length); // Key length, value length, equal sign, and null
                             // terminator
  memset(key_value_pair, 0, result_length);
  snprintf(key_value_pair, result_length, "%s=%s", key, value);
  vector_push_back(environment_get_env_vars_vector(), &key_value_pair);
}

void environment_get_available_variables(vector *output) {
  vector_iterator iterator;
  vector_iterator_initialize(&iterator, &environment_variables);
  while (vector_iterator_move_next(&iterator)) {
    string target = *(string *)vector_iterator_current_element(&iterator);
    char *kv_seperator = strchr(target, '=');
    if (kv_seperator == NULL) {
      perror(
          "[NON FATAL] found incorrectly formatted environment variable kvp\n");
      continue;
    }
    size_t key_length = (size_t)(kv_seperator - target);
    char *local_result = malloc(key_length + 1);
    memset(local_result, 0, key_length + 1);
    memcpy(local_result, target, key_length);
    vector_push_back(output, &local_result);
  }
}

string environment_raw_key_to_varref_format(string key) {
  size_t key_length = strlen(key);
  size_t result_length =
      key_length + 1 + 1; // Dollar sign, key length, null terminator
  string result = malloc(result_length);
  snprintf(result, result_length, "$%s", key);
  return result;
}

char *environment_apply_environment_variables(char *input) {
  vector keys;
  char *result = input;
  vector_initialize(&keys, sizeof(string));
  environment_get_available_variables(&keys);
  vector_iterator iterator;
  vector_iterator_initialize(&iterator, &keys);
  while (vector_iterator_move_next(&iterator)) {
    string key = *(string *)vector_iterator_current_element(&iterator);
    string varref_fmt_key = environment_raw_key_to_varref_format(key);
    string value = environment_get_variable_value(key);
    result = str_replace(result, varref_fmt_key, value);
  }
  return result;
}
