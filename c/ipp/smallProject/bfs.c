#include "bfs.h"
#include "queue.h"
#include "libs.h"
#include "twobitset.h"
#include "defines.h"

// Struct holding information needed for bfs algorithm
typedef struct bfs_info Bfs_info;

struct bfs_info {
    Queue queue;
    Twobitset visited;
    size_t point;
    size_t point_type;
    size_t path_length;
};

// Flips from type from 1 to 2 or 2 to 1
static uint8_t flip_visited_type(uint8_t type) {
    // Due to how type is represented xor with 0b11 flips values
    return type ^ 0b11;
}

// Returns type of point
static uint8_t get_point_type(Twobitset visited, size_t point) {
    return twobitset_get(visited, point);
}

// Adds to visited of specific type
static void add_visited(Twobitset visited, size_t point, uint8_t type) {
    twobitset_insert(visited, type, point);

    return;
}

// Checks if point is visited or partially visited
static bool is_visited(Twobitset visited, size_t point) {
    return twobitset_get(visited, point);
}

// Checks if point is nor a wall or visited
static bool is_available(Twobitset visited, size_t point, Lab_info *lab_info) {
    return !is_wall(point, lab_info) && !is_visited(visited, point);
}

// Add point to queue and set as partially visited
static void add_next(Bfs_info *bfs_info, size_t point) {
    queue_insert(bfs_info->queue, point);
    add_visited(bfs_info->visited, point, flip_visited_type(bfs_info->point_type));

    return;
}

// Adds all neighbors of point to be checked next
static void add_neighbors(Bfs_info *bfs_info, Lab_info *lab_info) {
    
    size_t point_part = 0;
    size_t current_multiplication = 1;
    // Value used to extract dimention from point
    size_t modified_point = bfs_info->point; 
    size_t new_point = 0;

    for (size_t i = 0; i < lab_info->dimentions->size; i++) {
        point_part = modified_point % lab_info->dimentions->values[i] + 1;
        modified_point /= lab_info->dimentions->values[i];
        
        // Substracting or adding current_multiplication from point creates 
        // a neighboring point
        if (point_part > 1) {
            new_point = bfs_info->point - current_multiplication;
            if (is_available(bfs_info->visited, new_point, lab_info)) {
                add_next(bfs_info, new_point);
            }
        }
        if (point_part < lab_info->dimentions->values[i]) {
            new_point = bfs_info->point + current_multiplication;
            if (is_available(bfs_info->visited, new_point, lab_info)) {
                add_next(bfs_info, new_point);
            }
        }

        current_multiplication *= lab_info->dimentions->values[i];
    }
}

size_t bfs(Lab_info *lab_info, bool* path_found) {
    Bfs_info bfs_info;

    // Last point will be set to visited type one, because path shouldn't be
    // incremented in first iteration of while loop
    uint8_t last_point_type = VISITED_TYPE_ONE;

    bfs_info.path_length = 0;
    bfs_info.queue = create_queue();
    bfs_info.visited = create_twobitset(lab_info->size);
    // Starting position is added with VISITED_TYPE_TWO so it will flip to
    // type 1
    bfs_info.point_type = VISITED_TYPE_TWO;

    add_next(&bfs_info, lab_info->start_point);

    *path_found = false;

    while(!queue_is_empty(bfs_info.queue)) {
        bfs_info.point = queue_front(bfs_info.queue);
        bfs_info.point_type = get_point_type(bfs_info.visited,  bfs_info.point);

        // Increment path length only when chaning point types
        if (last_point_type != bfs_info.point_type) {
            bfs_info.path_length++;
        }

        // Exit loop if path is found
        if (bfs_info.point == lab_info->end_point) {
            *path_found = true;
            break;
        }

        add_neighbors(&bfs_info, lab_info);

        queue_pop(bfs_info.queue);
        
        last_point_type = bfs_info.point_type;
    }

    free_queue(bfs_info.queue);
    free_twobitset(bfs_info.visited);

    return bfs_info.path_length;
}