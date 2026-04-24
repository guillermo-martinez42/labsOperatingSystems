#include <stdio.h>
#include "../process/process.h"
#include "queue.h"

void init_queue(Queue *q)
{
    q->front = NULL;
    q->back = NULL;
}

void enqueue(Queue *q, Thread *curr_thread)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
        return; // Allocation failed

    new_node->thread = curr_thread;
    new_node->next = NULL;

    if (q->back == NULL)
    {
        q->front = q->back = new_node;
        return;
    }

    q->back->next = new_node;
    q->back = new_node;
}

Thread *dequeue(Queue *q)
{
    if (q->front == NULL)
        return NULL;

    Node *curr = q->front;
    Thread *thread = curr->thread;

    q->front = q->front->next;

    // If front becomes NULL, back must also be NULL
    if (q->front == NULL)
    {
        q->back = NULL;
    }

    free(curr);
    return thread;
}

// For Round Robin
void requeue(Queue *q)
{
    if (q->front == NULL)
        return;

    Node *curr = q->front;
    q->front = q->front->next;

    enqueue(q, curr->thread);

    free(curr); // test
}

// For SJF & SRTF
void insert_SJF(Queue *q, Thread *thread)
{
    Node *curr = q->front;
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->thread = thread;

    // Insert in front
    if (curr == NULL || curr->thread->burst_time > thread->burst_time)
    {
        new_node->next = curr;
        curr = new_node;
    }
    else
    {
        // Insert in order
        while (curr->next != NULL && curr->next->thread->burst_time <= thread->burst_time)
        {
            curr = curr->next;
        }
        new_node->next = curr->next;
        curr->next = new_node;
    }
}

// For SRTF
Thread *get_SRTF(Queue *q)
{
    if (q->front == NULL)
        return NULL;

    Node *prev = NULL, *curr = q->front;
    Node *min_prev = NULL, *min_node = curr;

    // Search for the thread with shortest remaining time
    while (curr->next != NULL)
    {
        prev = curr;
        curr = curr->next;
        if (curr->thread->remaining_time < min_node->thread->remaining_time)
        {
            min_node = curr;
            min_prev = prev;
        }
    }
    
    // Unlinking the node to free it
    if (min_prev == NULL)
        curr = curr->next;
    else
        min_prev->next = min_node->next;

    Thread *thread = min_node->thread;
    free(min_node);
    return thread;
}