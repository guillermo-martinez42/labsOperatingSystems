#include "../process/process.h"
#include "queue.h"

void init_queue(Queue *q)
{
    q->front = NULL;
    q->back = NULL;
    pthread_mutex_init(&q->lock, NULL);
}

void enqueue(Queue *q, Thread *curr_thread)
{
    pthread_mutex_lock(&q->lock);
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        pthread_mutex_unlock(&q->lock);
        return; // Allocation failed
    }

    new_node->thread = curr_thread;
    new_node->next = NULL;

    if (q->back == NULL)
    {
        q->front = q->back = new_node;
        pthread_mutex_unlock(&q->lock);
        return;
    }

    q->back->next = new_node;
    q->back = new_node;
    pthread_mutex_unlock(&q->lock);
}

Thread *dequeue(Queue *q)
{
    pthread_mutex_lock(&q->lock);
    if (q->front == NULL)
    {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }

    Node *curr = q->front;
    Thread *thread = curr->thread;

    q->front = q->front->next;

    // If front becomes NULL, back must also be NULL
    if (q->front == NULL)
    {
        q->back = NULL;
    }

    free(curr);
    pthread_mutex_unlock(&q->lock);
    return thread;
}

// For SJF & SRTF
void insert_SJF(Queue *q, Thread *thread)
{
    pthread_mutex_lock(&q->lock);
    Node *new_node = (Node *)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        pthread_mutex_unlock(&q->lock);
        return;
    }
    new_node->thread = thread;

    // Insert in front
    if (q->front == NULL || q->front->thread->remaining_time > thread->remaining_time)
    {
        new_node->next = q->front;
        q->front = new_node;
        if (q->back == NULL)
            q->back = new_node;
    }
    else
    {
        // Insert in order
        Node *curr = q->front;
        while (curr->next != NULL && curr->next->thread->remaining_time <= thread->remaining_time)
        {
            curr = curr->next;
        }
        new_node->next = curr->next;
        curr->next = new_node;
        if (new_node->next == NULL)
            q->back = new_node;
    }
    pthread_mutex_unlock(&q->lock);
}

// For SRTF
Thread *get_SRTF(Queue *q)
{
    pthread_mutex_lock(&q->lock);
    if (q->front == NULL)
    {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }

    Node *min_prev = NULL;
    Node *min_node = q->front;
    Node *prev = q->front;
    Node *curr = q->front->next;

    // Search for the thread with shortest remaining time
    while (curr != NULL)
    {
        if (curr->thread->remaining_time < min_node->thread->remaining_time)
        {
            min_node = curr;
            min_prev = prev;
        }
        prev = curr;
        curr = curr->next;
    }

    // Unlinking the node to free it
    if (min_prev == NULL)
        q->front = min_node->next;
    else
        min_prev->next = min_node->next;

    if (min_node == q->back)
        q->back = min_prev;

    Thread *thread = min_node->thread;
    free(min_node);
    pthread_mutex_unlock(&q->lock);
    return thread;
}