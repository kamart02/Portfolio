#include "libs.h"
#include "twobitset.h"
#include "input.h"
#include "defines.h"
#include "error.h"
#include "conversions.h"
#include "vector.h"
#include "labyrinth.h"

// Checks if overflow will occur while a and b multiplication
static bool check_overflow_x(size_t a, size_t b) {
    size_t allowed_b;
    allowed_b = SIZE_MAX / a;

    return b > allowed_b;
}

bool check_dims_max_size(Lab_info *lab_info) {
    lab_info->size = 1;

    for (size_t i = 0; i < lab_info->dimentions->size; i++) {
        if (!check_overflow_x(lab_info->size, lab_info->dimentions->values[i])) {
            lab_info->size *= lab_info->dimentions->values[i];
        }
        else {
            return false;
        }
    }

    return true;
}

void check_if_true(bool val, int line, bool* err) {
    if (!*err && !val) {
        line_error(line, err);
    }
}

void check_point(Vector point, Lab_info *lab_info, int line, bool* err) {
    if (!*err) {
        // If a point dimentions amount is not equal to labrinth dimentions its not valid
        if (lab_info->dimentions->size != point->size) {
            line_error(line, err);

            return;
        }

        for (size_t i = 0; i < lab_info->dimentions->size; i++) {
            if (lab_info->dimentions->values[i] < point->values[i] || point->values[i] == 0) {
                line_error(line, err);
            }
        }
    }

    return;
}

int get_input_type(void) {
    char letter;

    // Discard all whitespaces other than NL or EOF
    do {
        letter = getchar();
    } while (isspace(letter) && letter != EOF && letter != NL);

    // First type of input
    if (letter == '0') {
        letter = getchar();
        
        // Zero must be followed by x
        if (letter == 'x') {
            return 1;
        }
    }
    // Second type of input
    else if (letter == 'R') {
        return 2;
    }

    
    return 0;
}

bool validate_1_input(String str, Lab_info *lab_info) {
    if (!is_hexadecimal(str)) {
        return false;
    }
    
    size_t bitnumber;
    // Checks fo empty string and if its size is greater than possible labirynth size
    // One char in string encodes 4 walls
    if (str->size == 0 || str->size > SIZE_MAX / 4) {
        return false;
    }

    // Finding the biggest bitnumber representing a wall 
    // Each char represents 4 points in maze
    // Bitnumber is set to represent the biggest wall by str without the first letter
    bitnumber = ((str->size) - 1) * 4;

    // Finding the most significant bit in the first char of str
    // and adding its coresponding position to bitnubmer to get
    // the biggest bitnumber representing a wall
    char bitmask = 0b1000;
    for (int i = 3; i >= 0; i--) {
        if (str->chars[0] & bitmask) {
            bitnumber += i;
            break;
        }
        bitmask >>= 1;
    }
    // Size of labyrinth -1 is the biggest bitnumber representing a point
    // If bitnumber is greater than the biggest point then it points to a 
    // point outside the labirinth
    return bitnumber <= lab_info->size - 1;
}

Twobitset parse_1_input(String str) {
    // Walls' size is set to 4 times the str size because one char in string
    // represents 4 points
    Twobitset walls = create_twobitset(str->size * 4);

    char number;
    size_t index;

    for (size_t i = str->size; i > 0; i--) {
        number = str->chars[i - 1];
        index = str->size - i;
        
        // Converting char resperentation of a number to decimal
        if (number >= 'a') {
            number -= 'a';
            number += 10;
        }
        else if (number >= 'A') {
            number -= 'A';
            number += 10;
        }
        else {
            number -= '0';
        }

        // Checking set bits of number to find all walls 
        for (int j = 0; j < 4; j++) {
            // Checking bits
            if (number & (0b0001 << j)) {
                // Index is multiplied by 4 because one char holds 4 bits and
                // offset by j
                twobitset_insert(walls, WALL, index * 4 + j);
            }   
        }
    }

    return walls;
}

bool validate_2_input(Vector numbers) {
    // Second type of input must be of size 5
    if (numbers->size != 5) {
        return false;
    }
    // Each number cannot be greater than UINT32_MAX
    for (size_t i = 0; i < numbers->size; i++) {
        if (numbers->values[i] > UINT32_MAX) {
            return false;
        }
    }

    return true;
}

Twobitset parse_2_input(Vector numbers, size_t labyrinth_size) {
    // Naming of variables is the same as in task description
    size_t a, b, m, r, s, w; 

    Twobitset walls;

    // Size of walls cannot be greater than 2^32
    if (labyrinth_size > TWO_TO_THIRTY_TWO) {
        walls = create_twobitset(TWO_TO_THIRTY_TWO);
    }
    else {
        walls = create_twobitset(labyrinth_size);
    }

    a = numbers->values[0];
    b = numbers->values[1];
    m = numbers->values[2];
    r = numbers->values[3];
    s = numbers->values[4];

    for (size_t i = 0; i < r; i++) {
        s = (a * s + b) % m;
        w = s % labyrinth_size;
        twobitset_insert(walls, WALL, w);
    }

    return walls; 
}

void clean_points(Vector *dims, Vector *start, Vector *end) {
    Vector new_dims;
    Vector new_start;
    Vector new_end;

    new_dims = create_vector();
    new_start = create_vector();
    new_end = create_vector();

    for (size_t i = 0; i < (*dims)->size; i++) {
        if ((*dims)->values[i] != 1) {
            vector_insert(new_dims, (*dims)->values[i]);
            vector_insert(new_start, (*start)->values[i]);
            vector_insert(new_end, (*end)->values[i]);
        }
    }

    free_vector(*dims);
    free_vector(*start);
    free_vector(*end);

    *dims = new_dims;
    *start = new_start;
    *end = new_end;

    return; 
}

bool is_wall(size_t bitnumber, Lab_info *lab_info) {
    // If input is of type 2 bitnumber needs to be modulated as per 
    // task specification
    if (lab_info->input_type == 2) {
        bitnumber %= TWO_TO_THIRTY_TWO;
    }

    if (bitnumber >= lab_info->walls->size) {
        return false;
    }

    return twobitset_get(lab_info->walls, bitnumber);
}

void remove_leading_zeros(String *str) {
    String new_str = create_string();
    size_t zero_number = 0;

    // Calculate number of leading zeros in str
    for (size_t i = 0; i < (*str)->size; i++) {
        if ((*str)->chars[i] == '0') {
            zero_number++;
        }
        else {
            break;
        }
    }
    // Copy to new string without zeros
    for (size_t i = zero_number; i < (*str)->size; i++) {
        string_insert(new_str, (*str)->chars[i]);
    }

    // If str was only '0' then one is added
    if (new_str->size == 0) {
        string_insert(new_str, '0');
    }

    free_string(*str);

    *str = new_str;

    return;
}

void check_if_wall_inside_start_end(Lab_info *lab_info, bool* err) {
    if (!*err) {
        // Check if start point is in a wall
        if (is_wall(lab_info->start_point, lab_info)) {
            line_error(2, err);
        } 
        // Check if end point is in a wall
        if (is_wall(lab_info->end_point, lab_info)) {
            line_error(3, err);
        }
    }

    return;
}