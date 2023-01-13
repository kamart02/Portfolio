#include "twobitset.h"
#include "error.h"

Twobitset create_twobitset(size_t size) {
    Twobitset set = safe_malloc(sizeof(struct twobitset)); 
    // Actual size of values array (arythetic done to round up to nearest integer)
    size_t actual_size = (size + 3) / 4;

    set->size = size;
    
    set->values = safe_calloc(actual_size, sizeof(uint8_t));

    return set;
}

void free_twobitset(Twobitset set) {
    if (set != NULL) {
        if (set->values != NULL) {
            free(set->values);
        }
        free(set);
    }

    return;
}

void twobitset_insert(Twobitset set, uint8_t value, size_t position) {
    // Index of value and offset used to insert bits into correct position
    size_t index = position / 4;
    size_t offset = position % 4;
    // Offset is multiplied by to because set holds two bits per position
    offset *= 2;

    // Extract only last two bits in value
    value &= 0b11;

    set->values[index] |= value << offset;
    
    return;
}

uint8_t twobitset_get(Twobitset set, size_t position) {
    // Index of value and offset used to insert bits into correct position
    size_t index = position / 4;
    size_t offset = position % 4;

    // Offset is multiplied by to because set holds two bits per position
    offset *= 2;

    // Return value is ANDed with 0b11 to extract only last two bits;
    return (set->values[index] >> offset) & 0b11;
}