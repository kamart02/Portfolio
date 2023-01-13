#ifndef BFS_H
#define BFS_H

#include "labyrinth.h"
#include "libs.h"

// Runs bfs using two types of visited point
// Updates path_foud if path is found
size_t bfs(Lab_info *lab_info, bool* path_found);

#endif