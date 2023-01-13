#ifndef LABYRINTH_H
#define LABYRINTH_H

#include "vector.h"
#include "input.h"
#include "twobitset.h"

// Representation of labyrinth
typedef struct lab_info Lab_info;

struct lab_info {
    Vector dimentions;
    size_t start_point;
    size_t end_point;
    size_t size;
    Twobitset walls;
    int input_type;  // Type of input in LINE 4
};

// Check if size of labrinth is greater than SIZE_MAX
// Calculates size
bool check_dims_max_size(Lab_info *lab_info);

// Runs line_error(line) if value is not true, updates err
void check_if_true(bool val, int line, bool* err);

// Checks if point is within ragne (0, dimention size] 
// Runs line_error(line) if is not
void check_point(Vector point, Lab_info *lab_info, int line, bool* err);

// Gets input type of LINE 4
// Returns 1 or 2 if input type is 1 (0x) or 2 (R)
// Return 0 if input type is invalid
int get_input_type(void);

// Validates 1st input
bool validate_1_input(String str, Lab_info *lab_info);

// Validates 2nd input
bool validate_2_input(Vector numbers);

// Parses 1st input and returns it as a Twobitset
// Str must be valid
Twobitset parse_1_input(String str);

// Parses 2nd input and returns it as a Twobitset
// Numbers must be valid
Twobitset parse_2_input(Vector numbers, size_t labyrinth_size) ;

// Removes all dimentions of size 1
// If all dimentions are of size 1 changes all vectors to be empty
void clean_points(Vector *dims, Vector *start, Vector *end);

// Checks if bitnumber is a wall
bool is_wall(size_t bitnumber, Lab_info *lab_info);

// Removes leading zeros from string
void remove_leading_zeros(String *str);

// Checks if start and end point is inside a wall
// Runs line_error for approriate line if error occurs
void check_if_wall_inside_start_end(Lab_info *lab_info, bool* err);


#endif

