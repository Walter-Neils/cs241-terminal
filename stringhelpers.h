#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H

#include <stdbool.h>

char* str_replace(char* orig, char* target, char* with);

bool str_equal(const char* str1, const char* str2);

#endif
