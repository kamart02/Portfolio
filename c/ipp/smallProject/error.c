#include "libs.h"
#include "defines.h"
#include "error.h"

// Checks if pointer is NULL if yes exits program
static void null_ptr_check(void* ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "%s\n", "ERROR 0");
        exit(1);
    }

    return;
}

void* safe_malloc(size_t size) {
    void *ptr = malloc(size);

    null_ptr_check(ptr);

    return ptr;
}

void* safe_realloc(void* ptr, size_t size) {
    void* new_ptr = realloc(ptr, size);

    null_ptr_check(new_ptr);

    return new_ptr;
}

void* safe_calloc(size_t size, size_t object_size) {
    void* ptr = calloc(size, object_size);

    null_ptr_check(ptr);

    return ptr;
}

void line_error(int line, bool* err) {
    if (!*err) {
        *err = true;
        fprintf(stderr, "%s%i\n", "ERROR ", line);
    }
    
    return;
}