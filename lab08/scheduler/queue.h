#include <stdio.h>
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
} Queue;

void init_queue(Queue *q);
void enqueue(Queue *q, Thread *curr_thread);
Thread *dequeue(Queue *q);

// For Round Robin
void requeue(Queue *q);

// For SJF & SRTF
void insert_SJF(Queue *q, Thread *thread);

// For SRTF
Thread *get_SRTF(Queue *q);