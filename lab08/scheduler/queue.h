#if !defined(QUEUE_H)
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../process/process.h"

// Represents a single element in the queue
typedef struct Node
{
    Thread *thread;
    struct Node *next;
} Node;

// Manages the front and back of the queue
typedef struct
{
    Node *front; // aka head
    Node *back;
    pthread_mutex_t lock;
} Queue;

void init_queue(Queue *q);
void enqueue(Queue *q, Thread *curr_thread);
Thread *dequeue(Queue *q);

// For SJF & SRTF
void insert_SJF(Queue *q, Thread *thread);

// For SRTF
Thread *get_SRTF(Queue *q);

#endif // QUEUE_H