#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_MEMORY_USAGE_BYTES (256 * 1024) // 2 MB
#define CHUNK_SIZE (MAX_MEMORY_USAGE_BYTES / sizeof(int) / 2) // Chunk size to keep memory under 2 MB

// Struct to hold thread arguments
typedef struct {
    int *numbers;
    int start;
    int end;
    int prime_count;
} ThreadArgs;

// Optimized function to check if a number is prime
bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

// Thread function to count primes in a given range
void *countPrimes(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *)args;
    thread_args->prime_count = 0;
    for (int i = thread_args->start; i < thread_args->end; i++) {
        if (isPrime(thread_args->numbers[i])) {
            thread_args->prime_count++;
        }
    }
    pthread_exit(NULL);
}

int main() {
    // Determine number of CPU cores
    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cores <= 0) num_cores = 1; // Fallback to 1 if detection fails
    const int NUM_THREADS = num_cores;

    int num;
    int total_counter = 0;

    // Allocate memory for chunk of numbers
    int *numbers = (int *)malloc(CHUNK_SIZE * sizeof(int));
    if (numbers == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Use buffered input for faster reading
    char buffer[1024];
    setvbuf(stdin, buffer, _IOFBF, sizeof(buffer));

    // Read and process numbers in chunks
    while (1) {
        int num_count = 0;

        // Read a chunk of numbers
        while (num_count < CHUNK_SIZE && scanf("%d", &num) != EOF) {
            numbers[num_count++] = num;
        }

        if (num_count == 0) break; // No more numbers to process

        // Create threads
        pthread_t threads[NUM_THREADS];
        ThreadArgs thread_args[NUM_THREADS];
        int range = num_count / NUM_THREADS;

        for (int i = 0; i < NUM_THREADS; i++) {
            thread_args[i].numbers = numbers;
            thread_args[i].start = i * range;
            thread_args[i].end = (i == NUM_THREADS - 1) ? num_count : (i + 1) * range;
            if (pthread_create(&threads[i], NULL, countPrimes, (void *)&thread_args[i]) != 0) {
                fprintf(stderr, "Error creating thread\n");
                free(numbers);
                return 1;
            }
        }

        // Wait for threads to finish
        for (int i = 0; i < NUM_THREADS; i++) {
            if (pthread_join(threads[i], NULL) != 0) {
                fprintf(stderr, "Error joining thread\n");
                free(numbers);
                return 1;
            }
            total_counter += thread_args[i].prime_count;
        }

        if (num_count < CHUNK_SIZE) break; // Reached end of input
    }

    // Output results
    printf("%d total primes.\n", total_counter);

    // Free memory
    free(numbers);

    return 0;
}
