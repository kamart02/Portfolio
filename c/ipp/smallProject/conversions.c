#include "libs.h"
#include "conversions.h"
#include "vector.h"
#include "string.h"
#include <errno.h>

size_t convert_to_ull(String str, bool* err) {
    size_t number;
    char** end = NULL;
    *err = false;

    number = strtoull(str->chars, end, 10);

    if (errno == ERANGE) {
        *err = true;
    }

    return number;
}

bool is_decimal(String str) {
    for (size_t i = 0; str->chars[i] != '\0'; i++) {
        if (!isdigit(str->chars[i])) {
            return false;
        }
    }

    return true;
}

bool is_hexadecimal(String str) {
    for (size_t i = 0; str->chars[i] != '\0'; i++) {
        if (!isxdigit(str->chars[i])) {
            return false;
        }
    }

    return true;
}

// Converts to the position of a bit which is representing the point in first type of LINE 4 input
size_t point_to_bitnumber(Vector vector, Vector dimensions) {
    size_t n = 1;
    size_t bitnumber = 0;

    for (size_t i = 0; i < vector->size; i++) {
        bitnumber += (vector->values[i] - 1) * n;
        n *= dimensions->values[i];
    }

    return bitnumber;
}
