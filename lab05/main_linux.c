#include "log_processor.h"

int main(int argc, char* argv[]) {
    const char* filename = "./access.log";
    
    if (argc > 1) {
        filename = argv[1];
    }
    
    printf("=== Multi-threaded Log Processor ===\n");
    printf("Processing file: %s\n\n", filename);
    
    printf("--- Running with 1 thread ---\n");
    run_benchmark(filename, 2);
    

    
    printf("\n--- Running with 4 threads ---\n");
    run_benchmark(filename, 6);
    
    return 0;
}
