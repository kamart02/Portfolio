#ifndef TWOBITSET_H
#define TWOBITSET_H

#include "libs.h"

// Bitset able to hold two bits of information per one index
typedef struct twobitset *Twobitset;

// Twobitset holds an 8bit integer for storage of bits
struct twobitset {
    size_t size;
    uint8_t* values;
};

// Insert two bits into twobitset
void twobitset_insert(Twobitset set, uint8_t value, size_t position);

// Get bits from twobitset
uint8_t twobitset_get(Twobitset set, size_t position);

// Create twobitset initialized to 0
Twobitset create_twobitset(size_t size);

// Free twobitset from memory
void free_twobitset(Twobitset set);

#endif