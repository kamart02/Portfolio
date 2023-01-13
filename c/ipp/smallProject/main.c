#include "libs.h"
#include "labyrinth.h"
#include "error.h"
#include "conversions.h"
#include "defines.h"
#include "vector.h"
#include "input.h"
#include "bfs.h"

int main() {

    // ** pointers definitions **
    Lab_info lab_info;

    Vector start_point;
    Vector end_point;
    Vector five_numbers;

    Flags input_flags;
    String str;

    // ** variables definitions **
    
    size_t path = 0;

    bool err = false;
    bool path_found = false;
    int end_of_input_type = 0;
    
    // ** variables initialization **
    start_point = NULL;
    end_point = NULL;
    input_flags = create_flags();
    str = NULL;
    five_numbers = NULL;
    lab_info.size = 1;
    lab_info.dimentions = NULL;
    lab_info.walls = NULL;
    
    // ** LINE 1 **
    lab_info.dimentions = input_line_of_numbers(input_flags);
    check_if_true(!input_flags->eof, 1, &err);
    check_if_true(!input_flags->err, 1, &err);

    if (!check_dims_max_size(&lab_info)) {
        line_error(0, &err);
    }
    if (lab_info.size == 0) {
        line_error(1, &err);
    }

    // ** LINE 2 **
    start_point = input_line_of_numbers(input_flags);
    check_if_true(!input_flags->eof, 2, &err);
    check_if_true(!input_flags->err, 2, &err);
    check_point(start_point, &lab_info, 2, &err);

    // ** LINE 3**
    end_point = input_line_of_numbers(input_flags);
    check_if_true(!input_flags->eof, 3, &err);
    check_if_true(!input_flags->err, 3, &err);
    check_point(end_point, &lab_info, 3, &err);

    clean_points(&lab_info.dimentions, &start_point, &end_point);

    lab_info.start_point = point_to_bitnumber(start_point, lab_info.dimentions);
    lab_info.end_point = point_to_bitnumber(end_point, lab_info.dimentions);

    // ** LINE 4 **
    if (!err) {
        lab_info.input_type = get_input_type();

        if (lab_info.input_type == 1) {
            str = input_string(input_flags);
            check_if_true(!input_flags->whitespace_beginig, 4, &err);
            remove_leading_zeros(&str);

            if (validate_1_input(str, &lab_info)) {
                lab_info.walls = parse_1_input(str);
            }
            else {
                line_error(4, &err);
            }

        }
        else if (lab_info.input_type == 2) {
            five_numbers = input_line_of_numbers(input_flags);
            if (validate_2_input(five_numbers)) {
                lab_info.walls = parse_2_input(five_numbers, lab_info.size);
            }
            else {
                line_error(4, &err);
            }
        }
        else {
            line_error(4, &err);
        }

        check_if_wall_inside_start_end(&lab_info, &err);
        if (!check_end_of_input(input_flags, &end_of_input_type)) {
            if (end_of_input_type == 1) {
                line_error(4, &err);
            }
            else if (end_of_input_type == 2) {
                line_error(5, &err);
            }
        }
    }

    // ** BFS (solving of the labyrinth) **
    if (!err) {
        path = bfs(&lab_info, &path_found);

        if (path_found) {
            printf("%zu\n", path);
        }
        else {
            printf("%s\n", "NO WAY");
        }
    }

    // ** releasing memory **  
    free_vector(lab_info.dimentions);
    free_vector(start_point);
    free_vector(end_point);
    free_vector(five_numbers);
    free_twobitset(lab_info.walls);
    free_string(str);
    free(input_flags);

    return err ? 1 : 0;
}