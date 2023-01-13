#include "libs.h"
#include "vector.h"
#include "error.h"

Vector create_vector(void) {
    Vector vector;

    vector = safe_malloc(sizeof(struct vector));
    vector->values = safe_malloc(sizeof(size_t) * 24);
    vector->size = 0; 
    vector->max_size = 24;
    
    return vector;
}

void free_vector(Vector vector) {
    if (vector != NULL) {
        if (vector->values != NULL) {
             free(vector->values);
        }
        free(vector);
    }

    return;
}

void vector_insert(Vector vector, size_t value) {
    if (vector->size == vector->max_size) {
        vector->max_size *= 2;
        vector->values = safe_realloc(vector->values, vector->max_size * sizeof(size_t));
    }
    vector->values[vector->size] = value;
    vector->size++;

    return;
}