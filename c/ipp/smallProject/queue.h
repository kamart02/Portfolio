#ifndef QUEUE_H
#define QUEUE_H

#include "libs.h"

typedef struct queue *Queue;

struct queue;

// Remove first value in queue
void queue_pop(Queue q);

// Insert value into queue
void queue_insert(Queue q, size_t value);

// Returns first value in queue
size_t queue_front(Queue q);

// Creates queue
Queue create_queue(void);

// Removes queue from memory
void free_queue(Queue q);

// Returns true if queue is empty
bool queue_is_empty(Queue q);

#endif