#ifndef INPUT_H
#define INPUT_H

#include "vector.h"
#include "string.h"

// Structure of inportant flags changed during input. Each one is true if its
// condition is met.
// nl - new line
// eof - end of file
// err - error
// empty - input is empty
typedef struct flags *Flags;

struct flags {
    bool nl;        
    bool eof;
    bool err;
    bool empty;
    bool whitespace_beginig;
};

// Creates flags
Flags create_flags(void);

// Inputs a string untill a new line, end of file or a whitespace after 
// a letter is read.
String input_string(Flags flags);

// Inputs a single decimal number.
// Returns 0 if an error occurs and sets flag.
size_t input_number(Flags flags);

// Inputs a line of numbers and sets flags.
// Returns a vector
Vector input_line_of_numbers(Flags flags);

// Checks if input has ended.
// If in the last read line there are more characters type is 1.
// Type is equal to 2 if there isn't EOF in the next line
bool check_end_of_input(Flags flags, int *type);

#endif