# Lab05 - Threads

#### By @GabGP and @guillermo-martinez42

This project aims to demonstrate the usage and benefits of multi-threading programming on a web log analyzer.

Functions:

- Counts the number of requets made by each unique IP address.
- Identifies the most frequently visited URL.
- Finds the number of HTTP errors (status codes 400-599).

This is done by reading an auto-generated access log file. Then depending on the amount of threads used, this file would get partitioned so, that each thread can process the data and ultimately, merging that data and getting it ready to be printed to the user.

![Alt text](https://github.com/guillermo-martinez42/labsOperatingSystems/blob/main/lab05/Screenshots/Example.png "Expected Output")

## 0. How to Run?

This program can be ran by running the shell script `run.sh`.

Note: This program is meant to be ran on Linux.

## 1. Access Log File Generator (access_log_file_generator.py)

This program is used to generate access log files.

## 2. Multi-Threaded Web Log Analyzer (log_processor.c/.h)

### File Partitioning

The file would get partitioned depending on the number of threads given. To get the total file size, fseek() and ftell() is used.

Then the chunk size needs to be established by dividing the file size by the number of threads.

After having done that, each chunk can be assigned to each partition that each thread would process.

Further read:

- https://man7.org/linux/man-pages/man3/fseek.3.html
- https://man7.org/linux/man-pages/man3/ftell.3p.html

### Thread Function (Worker)

This function intends to process each partition of the file. With that, we are looking to count the IPs and URLs, as well as the 400-599 errors.

We achieved this by using a data structure called ThreadData, which contains the filename, start and end byte of the file to be processed, IP and URL counters, and data structures to have a more detailed counter for each unique IP and URL.

### Synchronization (Merge)

A synchronization method was not really needed with our implementation, as each partition was processed separately and so now, what it is needed is just to merge together all this processed ThreadData into the result. This merging gets done after all threads have finished, this way the synchroization is avoided.

For this, we use a data structure named AggregatedResults. Basically, it contains the IP and URL counters, as well as the IPs, URLs and errors data. So it is a lite version of ThreadData, but without the filename and start and end byte of the partition.

### Aggregation and Reporting

This function aims to gather the relevant data from AggregatedResults and printing it to the user. This would include the total unique IPs, the most visited URL and the amount of HTTP errors.

### Performance Benchmark

This is in someways the main function of this program and this is also where the use of threads gets put into practice.

To use each thread, a thread has to be created with pthread_create(). This gets used to pass on the function to the thread and the arguments, in this case the ThreadData for each thread.

Only after having created them, then each thread gets joined with pthread_join(). What this does is it makes the main thread to wait for each thread to be completed, so it can then continue processing the program.

Otherwise unintended behaviour can occur, like the early termination of the main thread, which would have the incomplete work of the threads.

Further read: https://www.geeksforgeeks.org/c/thread-functions-in-c-c/

## 3. Main Program (main_linux.c)

This is the main program, that would start the whole program by passing on the filename, calling the run_benchmark function and give the number of threads.
