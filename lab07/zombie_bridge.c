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

Bridge bridge = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .condition = PTHREAD_COND_INITIALIZER,
    .students_on_bridge = 0,
    .crossing_direction = -1,
    .waiting = {0, 0},
    .total_wait_time = 0,
    .total_crossings = 0,
    .priority_waiters = {0, 0}
};

static __thread int tls_student_id = -1;

const char *direction_name(int direction)
{
    return direction == DIRECTION_RIGHT ? "Right" : "Left";
}

double get_avg_wait_time()
{
    return bridge.total_wait_time / bridge.total_crossings;
}

double get_current_wait_time(struct timespec now, struct timespec start)
{
    return (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1e9;
}

void log_arrival(int student_id, int direction)
{
    printf("Inge %02d arrives wanting to go %s\n", student_id, direction_name(direction));
}

void log_enter(int student_id, int direction, double wait_time)
{
    printf("Inge %02d crosses to the %s (on bridge: %d) [Wait: %.2fs | Avg: %.2fs]\n",
           student_id,
           direction_name(direction),
           bridge.students_on_bridge,
           wait_time,
           get_avg_wait_time());
}

void log_exit(int student_id)
{
    printf("Inge %02d exits bridge (on bridge: %d)\n", student_id, bridge.students_on_bridge);
}

void accessBridge(int direction)
{
    struct timespec arrival;
    clock_gettime(CLOCK_MONOTONIC, &arrival);

    pthread_mutex_lock(&bridge.mutex);

    bridge.waiting[direction]++;
    log_arrival(tls_student_id, direction);

    int has_priority = 0;
    int can_cross = 1;

    while (1)
    {
        if (!has_priority)
        {
            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            if (get_current_wait_time(now, arrival) >= PRIORITY_WAIT_THRESHOLD)
            {
                has_priority = 1;
                bridge.priority_waiters[direction]++;
            }
        }

        can_cross = 1;

        if (bridge.students_on_bridge == BRIDGE_CAPACITY)
        {
            can_cross = 0; // Bridge is full
        }
        else if (bridge.students_on_bridge > 0 && bridge.crossing_direction != direction)
        {
            can_cross = 0; // Bridge is currently moving the opposite way
        }
        else if (!has_priority && bridge.priority_waiters[direction] > 0)
        {
            can_cross = 0; // Yield those with priority
        }
        else if (has_priority && bridge.crossing_direction == direction)
        {
            can_cross = 1; // Has priority
        }
        else if (has_priority && bridge.crossing_direction != direction && bridge.students_on_bridge == 0)
        {
            can_cross = 1; // Starvation prevention
        }

        if (!can_cross)
        {
            pthread_cond_wait(&bridge.condition, &bridge.mutex);
        }
        else
        {
            break; 
        }
    }

    struct timespec enter;
    clock_gettime(CLOCK_MONOTONIC, &enter);
    
    double actual_wait = get_current_wait_time(enter, arrival);
    bridge.total_wait_time += actual_wait;
    bridge.total_crossings++;

    if (bridge.students_on_bridge == 0)
    {
        bridge.crossing_direction = direction;
    }

    bridge.waiting[direction]--;
    bridge.students_on_bridge++;

    if (has_priority)
    {
        bridge.priority_waiters[direction]--;
        printf("[PRIORITY] ");
    }

    log_enter(tls_student_id, direction, actual_wait);

    pthread_mutex_unlock(&bridge.mutex);
}

void exitBridge(void)
{
    pthread_mutex_lock(&bridge.mutex);

    bridge.students_on_bridge--;

    if (bridge.students_on_bridge == 0)
    {
        bridge.crossing_direction = -1;
    }

    log_exit(tls_student_id);

    /* Wake all waiters so eligible students can enter immediately. */
    pthread_cond_broadcast(&bridge.condition);

    pthread_mutex_unlock(&bridge.mutex);
}

void *student_thread(void *arg)
{
    StudentArgs *student = (StudentArgs *)arg;
    int id = student->id;
    unsigned int seed = (unsigned int)time(NULL) ^ ((unsigned int)id * 2654435761u);
    free(student);

    tls_student_id = id;
    int arrival_delay = rand_r(&seed) % 6;       /* 0..5 seconds */
    int direction = rand_r(&seed) % 2;           /* 0=right, 1=left */
    int crossing_time = (rand_r(&seed) % 3) + 1; /* 1..3 seconds */

    sleep((unsigned int)arrival_delay);
    accessBridge(direction);
    sleep((unsigned int)crossing_time);
    exitBridge();

    return NULL; /* Unreachable, kept for clarity. */
}

int main(void)
{
    pthread_t students[NUM_STUDENTS];

    for (int i = 0; i < NUM_STUDENTS; i++)
    {
        StudentArgs *args = (StudentArgs *)malloc(sizeof(StudentArgs));
        if (args == NULL)
        {
            perror("malloc");
            return EXIT_FAILURE;
        }

        args->id = i + 1;

        if (pthread_create(&students[i], NULL, student_thread, args) != 0)
        {
            perror("pthread_create");
            free(args);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_STUDENTS; i++)
    {
        if (pthread_join(students[i], NULL) != 0)
        {
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
