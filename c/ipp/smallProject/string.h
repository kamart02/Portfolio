#ifndef STRING_H
#define STRING_H

#include "libs.h"

// Null terminated string without whitespaces
typedef struct string *String;

struct string {
    size_t size;
    size_t max_size;  
    char* chars;
};

// Create a null terminated string
String create_string(void);

// Insert a character at the end of a string
void free_string(String s);

// Free string from memory
void string_insert(String s, char letter);

#endif