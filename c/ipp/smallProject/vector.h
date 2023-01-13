#ifndef VECTOR_H
#define VECTOR_H

#include "libs.h"

typedef struct vector *Vector;

struct vector {
    size_t max_size;    
    size_t size;
    size_t* values;
};

// Inserts a value at the end of vector
void vector_insert(Vector vector, size_t value);

// Creates vector with size_t values.
Vector create_vector(void);

// Frees vector from memory
void free_vector(Vector vector);

#endif
