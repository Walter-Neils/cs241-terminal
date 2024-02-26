#include "parser.h" 
#include <stdlib.h>
#include <string.h>


void tokenize_command(string line, vector *token_vector) {
  if(token_vector->element_size != sizeof(string)) {
    perror("Token vector is initialized for the incorrect type");
    exit(-1);
  }

  size_t line_length = strlen(line);

  char token_buffer[256];
  size_t result_current_index = 0;
  bool is_string_wrapped = false;
  bool is_escaped = false;
  for(size_t i = 0; i < line_length; i++) {
    if(result_current_index >= sizeof(token_buffer) / sizeof(char)) {
      perror("Impending buffer overflow: bailing out, good luck!");
      exit(-1);
    }
    char current_char = line[i];
    if(current_char == '"') {
      if(!is_escaped) {
        is_string_wrapped = !is_string_wrapped;
        i++;
        current_char = line[i];
      }
    }
    if(current_char == ' ' && !is_string_wrapped) {
      // This is the end of the current token
      token_buffer[result_current_index] = '\0';
      char* token_cold_storage = malloc(sizeof(char) * result_current_index);
      memcpy(token_cold_storage, token_buffer, sizeof(char) * result_current_index);
      vector_push_back(token_vector, &token_cold_storage);
      result_current_index = 0;
    }
    else if(current_char == '\\') {
      if(is_escaped) {
        token_buffer[result_current_index++] = '\\';
      }
      else {
        is_escaped = true;
        continue; // See the if(is_escaped) line below to understand
      }
    }
    else if(current_char == '\n') {
      // TODO: This might need to be treated as an error because a command can only ever be a single line thing
      continue; // Absolutely not
    }
    else {
      token_buffer[result_current_index++] = current_char;
    }


    if(is_escaped) {
      is_escaped = false;
    }
  }

  if(result_current_index > 0) {
    token_buffer[result_current_index] = '\0';
    char* token_cold_storage = malloc(sizeof(char) * result_current_index);
    memcpy(token_cold_storage, token_buffer, sizeof(char) * result_current_index);
    vector_push_back(token_vector, &token_cold_storage);
    result_current_index = 0;
  }
}
