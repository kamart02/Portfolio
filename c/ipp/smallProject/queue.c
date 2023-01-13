#include "queue.h"
#include "error.h"

// Struct for elemnts kept inside the queue
typedef struct element *Element;

struct element {
    Element next;
    size_t value;
};

struct queue {
    Element front;
    Element end;
};

// Creates and element
static Element create_element(size_t value) {
    Element elem;
    elem = safe_malloc(sizeof(struct element));
    elem->value = value;
    elem->next = NULL;

    return elem;
}

Queue create_queue() {
    Queue q;
    q = safe_malloc(sizeof(struct queue));
    q->front = NULL;
    q->end = NULL;

    return q;
}

bool queue_is_empty(Queue q) {
    return q->front == NULL;
}

void queue_insert(Queue q, size_t value) {
    Element elem;
    elem = create_element(value);

    if (q->front == NULL) {
        q->front = elem;
        q->end = elem;
    }
    else {
        q->end->next = elem;
        q->end = elem;
    }

    return;
}

size_t queue_front(Queue q) {
    return !queue_is_empty(q) ? q->front->value : 0;
}

void queue_pop(Queue q) {
    Element next;

    if (!queue_is_empty(q)) {
        next = q->front->next;
        free(q->front);

        if (next == NULL) {
            q->front = NULL;
            q->end = NULL;
        }
        else {
            q->front = next;
        }
    }

    return;
}

void free_queue(Queue q) {
    while(!queue_is_empty(q)) {
        queue_pop(q);
    }

    free(q);
    
    return;
}