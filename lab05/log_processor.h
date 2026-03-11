#ifndef LOG_PROCESSOR_H
#define LOG_PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_LINE_LENGTH 4096
#define MAX_IP_LENGTH 64
#define MAX_URL_LENGTH 512
#define MAX_THREADS 16
#define MAX_RESULTS 4096

typedef struct {
    char ip[MAX_IP_LENGTH];
    int count;
} IPCount;

typedef struct {
    char url[MAX_URL_LENGTH];
    int count;
} URLCount;

typedef struct {
    int error_4xx;
    int error_5xx;
} ErrorCount;

typedef struct {
    long start_byte;
    long end_byte;
    char* filename;
    IPCount* ip_results;
    int ip_count;
    URLCount* url_results;
    int url_count;
    ErrorCount error_count;
} ThreadData;

typedef struct {
    IPCount* ips;
    int ip_count;
    URLCount* urls;
    int url_count;
    ErrorCount errors;
} AggregatedResults;

void calculate_file_partitions(const char* filename, int num_threads, long partitions[]);

void* process_log_chunk(void* arg);

void merge_results(AggregatedResults* master, ThreadData* thread_results, int num_threads);

void report_results(AggregatedResults* results);

void free_thread_data(ThreadData* data, int num_threads);

void free_aggregated_results(AggregatedResults master);

void run_benchmark(const char* filename, int num_threads);

double get_elapsed_time(struct timeval start, struct timeval end);

#endif
