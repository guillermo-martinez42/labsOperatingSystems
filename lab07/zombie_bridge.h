#if !defined(ZOMBIE_BRIDGE_H)
#define ZOMBIE_BRIDGE_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_STUDENTS 20
#define BRIDGE_CAPACITY 4
#define DIRECTION_RIGHT 0
#define DIRECTION_LEFT 1
#define PRIORITY_WAIT_THRESHOLD 5.0

typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    int students_on_bridge;
    int crossing_direction; /* -1  is empty */
    int waiting[2];
    double total_wait_time;
    long total_crossings;
    int starving_direction;
    int priority_waiters[2];
} Bridge;

typedef struct
{
    int id;
} StudentArgs;

const char *direction_name(int direction);
double get_avg_wait_time();
double get_current_wait_time(struct timespec now, struct timespec start);
void log_arrival(int student_id, int direction);
void log_enter(int student_id, int direction, double wait_time);
void log_exit(int student_id);
void accessBridge(int direction);
void exitBridge(void);
void *student_thread(void *arg);
int main(void);

#endif // ZOMBIE_BRIDGE_H
