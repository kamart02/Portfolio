#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include "vector.h"
#include "string.h"

// Converts string to unsigned long long
// Updates err if not possible or number is bigger then SIZE_MAX
// Returns 0 if error occured
size_t convert_to_ull(String str, bool* err);

// Checks if string is a decimal number
bool is_decimal(String str);

// Checks if string is a hexadecimal number
bool is_hexadecimal(String str);

// Converts point to bitnumber (similar representation of point to first type of LINE 4 input)
size_t point_to_bitnumber(Vector vector, Vector dimensions);

#endif