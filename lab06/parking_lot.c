#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define NUM_CARS  10
#define NUM_SPOTS 3

sem_t parking_semaphore;
// TODO: initialize semaphore

pthread_mutex_t log_mutex   = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

int    total_cars_parked = 0;
double total_wait_time   = 0.0;

void log_event(int car_id, const char *message) {
    pthread_mutex_lock(&log_mutex);

    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[24] = '\0';
    printf("[%s] Car %d: %s\n", timestamp, car_id, message);

    pthread_mutex_unlock(&log_mutex);
}

void *car_thread(void *arg) {
    int car_id = *(int *)arg;
    free(arg);

    struct timespec arrival;
    clock_gettime(CLOCK_MONOTONIC, &arrival);

    log_event(car_id, "arrived at the parking lot");

    // TODO: wait on parking_semaphore (sem_wait)

    struct timespec parked;
    clock_gettime(CLOCK_MONOTONIC, &parked);
    double wait_time = (parked.tv_sec - arrival.tv_sec)
                     + (parked.tv_nsec - arrival.tv_nsec) / 1e9;

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "parked (waited %.3f seconds)", wait_time);
    // TODO: log successful park with wait time using log_event

    // TODO: sleep for a random 1–5 seconds to simulate parking duration

    // TODO: log departure using log_event

    // TODO: post parking_semaphore (sem_post)

    pthread_mutex_lock(&stats_mutex);
    total_cars_parked++;
    total_wait_time += wait_time;
    pthread_mutex_unlock(&stats_mutex);

    return NULL;
}

int main(void) {
    srand((unsigned int)time(NULL));

    // TODO: initialize semaphore (sem_init)

    pthread_t threads[NUM_CARS];

    for (int i = 0; i < NUM_CARS; i++) {
        int *id = malloc(sizeof(int));
        if (!id) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        *id = i + 1;
        pthread_create(&threads[i], NULL, car_thread, id);
    }

    for (int i = 0; i < NUM_CARS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n--- Parking Lot Statistics ---\n");
    printf("Total cars parked : %d\n", total_cars_parked);
    printf("Average wait time : %.3f seconds\n",
           total_cars_parked > 0 ? total_wait_time / total_cars_parked : 0.0);

    // TODO: destroy semaphore (sem_destroy)

    return 0;
}
