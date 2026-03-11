#include "log_processor.h"



/*
 * TASK 1: FILE PARTITIONING
 * -------------------------
 * calculate_file_partitions():
 * - Get total file size using fseek() and ftell()
 * - Divide file size by num_threads to get chunk size
 * - Set start_byte = thread_id * chunk_size
 * - Set end_byte = (thread_id == num_threads-1) ? file_size : (thread_id + 1) * chunk_size
 *
 */
void calculate_file_partitions(const char* filename, int num_threads, long* partitions) {
    
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

    if (data->start_byte != 0) {
        char skip[MAX_LINE_LENGTH];
        if (fgets(skip, MAX_LINE_LENGTH, file) == NULL) return NULL;
    }

    while (ftell(file) < data->end_byte) {
        char line[MAX_LINE_LENGTH];

        if (fgets(line, MAX_LINE_LENGTH, file) == NULL) break;

        char ip[MAX_IP_LENGTH];
        char url[MAX_URL_LENGTH];
        int status;

        if (sscanf(line, "%15s - - [%*[^]]] \"%*s %255[^\"]\" %d", ip, url, &status) != 3)
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
            data->error_count.error_4xx++;
        }
        if (status >= 500 && status < 600) {
            data->error_count.error_5xx++;
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
    for (int t = 0; t < num_threads; t++) {
        ThreadData* td = &thread_results[t];

        for (int i = 0; i < td->ip_count; i++) {
            int found = 0;
            for (int j = 0; j < master->ip_count; j++) {
                if (strcmp(master->ips[j].ip, td->ip_results[i].ip) == 0) {
                    master->ips[j].count += td->ip_results[i].count;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strcpy(master->ips[master->ip_count].ip, td->ip_results[i].ip);
                master->ips[master->ip_count].count = td->ip_results[i].count;
                master->ip_count++;
            }
        }

        for (int i = 0; i < td->url_count; i++) {
            int found = 0;
            for (int j = 0; j < master->url_count; j++) {
                if (strcmp(master->urls[j].url, td->url_results[i].url) == 0) {
                    master->urls[j].count += td->url_results[i].count;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strcpy(master->urls[master->url_count].url, td->url_results[i].url);
                master->urls[master->url_count].count = td->url_results[i].count;
                master->url_count++;
            }
        }

        master->errors.error_4xx += td->error_count.error_4xx;
        master->errors.error_5xx += td->error_count.error_5xx;
    }
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
    
    printf("Total Unique IPs: %d\n", results->ip_count);

    char most_visited_url[MAX_URL_LENGTH];
    int curr_url_count = 0;
    for (size_t i = 0; i < results->url_count; i++)
    {
        if (curr_url_count < results->urls[i].count)
        {
            strcpy(most_visited_url, results->urls[i].url);
            curr_url_count = results->urls[i].count;
        }
    }
    
    printf("Most Visited URL: %s (%d times)\n", most_visited_url, curr_url_count);

    int total_errors = results->errors.error_4xx + results->errors.error_5xx;
    printf("HTTP Errors: %d\n", total_errors);
    
}

/*
 * Helper: Free thread data
 */
void free_thread_data(ThreadData* thread_data, int num_threads) {

    for (size_t i = 0; i < num_threads; i++)
    {
        free(thread_data[i].ip_results);   
        free(thread_data[i].url_results);  
        free(thread_data[i].filename);
    }

    free(thread_data);  

}

/*
 * Helper: Free aggregated results
 */
void free_aggregated_results(AggregatedResults master) {

    free(master.ips);
    free(master.urls);

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
    
    long partitions[num_threads];
    calculate_file_partitions(filename, num_threads, partitions);

    ThreadData* thread_data = malloc(num_threads * sizeof(ThreadData));
    for (size_t i = 0; i < num_threads; i++)
    {
        thread_data[i].start_byte = (i == 0) ? 0 : partitions[i-1];
        thread_data[i].end_byte = partitions[i];
        thread_data[i].filename = strdup(filename);

        thread_data[i].ip_results = malloc(MAX_RESULTS * sizeof(IPCount));
        thread_data[i].ip_count = 0;

        thread_data[i].url_results = malloc(MAX_RESULTS * sizeof(URLCount));
        thread_data[i].url_count = 0;
        
        thread_data[i].error_count.error_4xx = 0;
        thread_data[i].error_count.error_5xx = 0;
    }

    struct timeval start;
    gettimeofday(&start, 0);

    pthread_t thread[num_threads];

    for (size_t i = 0; i < num_threads; i++)
    {
        pthread_create(&thread[i], NULL, process_log_chunk, &thread_data[i]);
    }

    for (size_t i = 0; i < num_threads; i++)
    {
        pthread_join(thread[i], NULL);
    }
    
    struct timeval end;
    gettimeofday(&end, 0);

    AggregatedResults results;
    results.ips = malloc(MAX_RESULTS * sizeof(IPCount));
    results.ip_count = 0;
    results.urls = malloc(MAX_RESULTS * sizeof(URLCount));
    results.url_count = 0;
    results.errors.error_4xx = 0;
    results.errors.error_5xx = 0;
    merge_results(&results, thread_data, num_threads);

    report_results(&results);

    double elapsed_time = get_elapsed_time(start, end);
    printf("Elapsed time: %f seconds\n", elapsed_time);

    free_thread_data(thread_data, num_threads);
    free_aggregated_results(results);
    
}
