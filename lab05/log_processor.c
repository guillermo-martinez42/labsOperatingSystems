#include "log_processor.h"

#include <stdio.h>
#include <stdlib.h>                 
#include <string.h>
#include <pthread.h>
#include <sys/time.h>


/*
 * TASK 1: FILE PARTITIONING
 * -------------------------
 * calculate_file_partitions():
 * - Get total file size using fseek() and ftell()
 * - Divide file size by num_threads to get chunk size
 * - Set start_byte = thread_id * chunk_size
 * - Set end_byte = (thread_id == num_threads-1) ? file_size : (thread_id + 1) * chunk_size
 * 
 * IMPORTANT: Adjust start_byte to skip to next newline character
 *            so threads don't start in the middle of a line
 */
void calculate_file_partitions(const char* filename, int num_threads, long partitions[]) {
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    long chunk_size = file_size / num_threads;
    for (int i = 0; i < num_threads; i++) {
        partitions[i] = i * chunk_size;
        if (i == num_threads - 1) {
            partitions[i] = file_size;
        }
    }
    fclose(file);   
}

/*
 * TASK 2: THREAD FUNCTION (WORKER)
 * --------------------------------
 * process_log_chunk():
 * - Open the file (passed in ThreadData)
 * - Seek to start_byte position
 * - While current position < end_byte:
 *   - Read a line (fgets)
 *   - Parse line to extract: IP address, URL, status code
 *     (format: IP - - [timestamp] "METHOD URL HTTP/x.x" STATUS)
 *   - Update local IP counter (add to ip_results)
 *   - Update local URL counter (add to url_results)
 *   - If status starts with 4xx, increment error_4xx
 *   - If status starts with 5xx, increment error_5xx
 * - Store results in ThreadData structure
 * - Return NULL
 * 
 * 
 */
void* process_log_chunk(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    FILE *file = fopen(data->filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, data->start_byte, SEEK_SET);

    // Skip partial first line if not at the start of the file
    if (data->start_byte != 0) {
        char skip[MAX_LINE_LENGTH];
        fgets(skip, MAX_LINE_LENGTH, file);
    }

    while (ftell(file) < data->end_byte) {
        char line[MAX_LINE_LENGTH];

        if (fgets(line, MAX_LINE_LENGTH, file) == NULL) break;

        char ip[MAX_IP_LENGTH];
        char url[MAX_URL_LENGTH];
        int status;

        // Parse: IP - - [timestamp] "METHOD URL HTTP/x.x" STATUS
        if (sscanf(line, "%15s - - [%*[^]]] \"%*s %255[^ ] %*[^\"]\" %d", ip, url, &status) != 3)
            continue;

        
        int found = 0;
        for (int i = 0; i < data->ip_count; i++) {
            if (strcmp(data->ip_results[i].ip, ip) == 0) {
                data->ip_results[i].count++;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(data->ip_results[data->ip_count].ip, ip);
            data->ip_results[data->ip_count].count = 1;
            data->ip_count++;
        }

        
        int url_found = 0;
        for (int i = 0; i < data->url_count; i++) {
            if (strcmp(data->url_results[i].url, url) == 0) {
                data->url_results[i].count++;
                url_found = 1;
                break;
            }
        }
        if (!url_found) {
            strcpy(data->url_results[data->url_count].url, url);
            data->url_results[data->url_count].count = 1;
            data->url_count++;
        }

       
        if (status >= 400 && status < 500) {
            data->error_count->error_4xx++;
        }
        if (status >= 500 && status < 600) {
            data->error_count->error_5xx++;
        }
    }

    fclose(file);
    return NULL;
}

/*
 * TASK 3: SYNCHRONIZATION (MERGE)
 * -------------------------------
 * merge_results():
 * - For each thread's results:
 *   - Merge IP counts into master (add counts for same IPs)
 *   - Merge URL counts into master (add counts for same URLs)
 *   - Add error_4xx and error_5xx to master totals
 * 
 * NOTE: This is done AFTER all threads complete (post-join)
 *       so no mutex needed - just single-threaded merging
 */
void merge_results(AggregatedResults* master, ThreadData* thread_results, int num_threads) {
    
}

/*
 * TASK 4: AGGREGATION AND REPORTING
 * ---------------------------------
 * report_results():
 * - Count unique IPs (ip_count in master)
 * - Find URL with highest count (iterate and find max)
 * - Print total HTTP errors (4xx + 5xx)
 * - Print all results in required format
 */
void report_results(AggregatedResults* results) {
    
}

/*
 * Helper: Free thread data
 */
void free_thread_data(ThreadData* data, int num_threads) {

    free(data->ip_results);   
    free(data->url_results);  
    free(data->error_count);
    free(data->filename);
    free(data);  

}

/*
 * Helper: Get elapsed time in seconds
 */
double get_elapsed_time(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + 
           (end.tv_usec - start.tv_usec) / 1000000.0;
}

/*
 * TASK 5: PERFORMANCE BENCHMARKING
 * --------------------------------
 * run_benchmark():
 * - For given number of threads:
 *   - Create ThreadData for each thread
 *   - Calculate file partitions
 *   - Start timer (gettimeofday)
 *   - Create and start all threads
 *   - Wait for all threads to complete (pthread_join)
 *   - Stop timer
 *   - Merge results from all threads
 *   - Report results
 *   - Print elapsed time
 *   - Free memory
 */
void run_benchmark(const char* filename, int num_threads) {
 
}
