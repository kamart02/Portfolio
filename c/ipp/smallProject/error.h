#ifndef ERROR_H
#define ERROR_H

#include "libs.h"

// Outputs error to stderr and updates err value to true
void line_error(int line, bool* err);

// Malloc with NULL pointer checking
// Outputs an error if pointer is NULL and ends program
void* safe_malloc(size_t size);

// Realloc with NULL pointer checking
// Outputs an error if pointer is NULL and ends program
void* safe_realloc(void* ptr, size_t size);

// Calloc with NULL pointer checking
// Outputs an error if pointer is NULL and ends program
void* safe_calloc(size_t size, size_t object_size);

#endif

