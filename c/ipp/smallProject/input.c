#include "libs.h"
#include "input.h"
#include "error.h"
#include "defines.h"
#include "conversions.h"
#include "string.h"

// Reset flags to its default values
static void reset_flags(Flags flags) {
    flags->err = false;
    flags->eof = false;
    flags->nl = false;
    flags->empty = true;
    flags->whitespace_beginig = false;
}

Flags create_flags(void) { 
    return safe_malloc(sizeof(struct flags));
}

String input_string(Flags flags) {
    char letter;

    String str;
    str = create_string();

    reset_flags(flags);
    
    while (true) {
        letter = getchar();

        if (letter == EOF) {
            flags->eof = true;
            break;
        }
        if (letter == NL) {
            flags->nl = true;
            break;
        }
        if (!isspace(letter)) {
            string_insert(str, letter);
            flags->empty = false;
        }
        else if (!flags->empty) {
            break;
        }
        else {
            flags->whitespace_beginig = true;
        }
    }

    return str;
}

size_t input_number(Flags flags) {
    size_t number = 0;
    String str = input_string(flags); 

    if ((flags->eof && flags->empty) || !is_decimal(str)) {
        flags->err = true;
    }
    else {
        number = convert_to_ull(str, &flags->err);
    }

    free_string(str);

    return number;
}

Vector input_line_of_numbers(Flags flags) {
    size_t number = 0;
    Vector number_vector = create_vector(); 

    do {
        number = input_number(flags);

        if(flags->err) {
            return number_vector;
        }

        if(!flags->empty) {
            vector_insert(number_vector, number);
        }

    } while (!(flags->eof || flags->nl));

    return number_vector;
}

bool check_end_of_input(Flags flags, int *type) {
    *type = 0;
    if (flags->eof) {
        return true;
    }

    // Sets type based on line (if a NL occured then error will be in 2nd line)
    if (flags->nl) {
        *type = 2;
    }
    else {
        *type = 1;
        // Immediately free string, because its contents are not needed
        // Input string only if still in 1st line
        free_string(input_string(flags));

        // Checks if something is in 1st line
        if (flags->eof) {
            *type = 0;
            return true;
        }
        else if (!flags->empty) {
            return false;
        }
    }

    // If string was empty then check next character (should be EOF)
    if (flags->nl) {
        char letter = getchar();

        if (letter != EOF) {
            *type = 2;
            return false;
        }
    }

    return true;
}