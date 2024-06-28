#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


// This program reads chanks of numbers from the input and counts the number of prime numbers in each chunk in parallel using multiple threads.
// It uses a fixed amount of memory to store the numbers and processes them in chunks to avoid memory overflow.
// Also uses the number of CPU cores to determine the number of threads to use for efficiency.
// The program uses buffered input to read numbers faster.
// And program uses a more optimized function to check if a number is prime. by checking only up to sqrt(n) instead of n/2. and only checking up to 6k+1 and 6k-1.
// Finally, program outputs the total number of prime numbers found at the end to comapre with the original code that does not use threads and memory manipulations.
// Authors: Yael Rosen & Itamar Kuznitsov.
// Since: Jun 24. 

// Maximoum amount of memory to use in bytes. Its always a trade-off between memory and speed - we found 256 KB to be the most efficient in our local machine
#define MAX_MEMORY_USAGE_BYTES (256 * 1024)
// Chunk size to keep memory under 2 MB
#define CHUNK_SIZE (MAX_MEMORY_USAGE_BYTES / sizeof(int) / 2)

// Struct to hold thread arguments
typedef struct {
    int *numbers;
    int start;
    int end;
    int prime_count;
} PrimeThreadArgs;

// Optimized function to check if a number is prime - O(sqrt(n))
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
    PrimeThreadArgs *thread_args = (PrimeThreadArgs *)args;
    thread_args->prime_count = 0;
    for (int i = thread_args->start; i < thread_args->end; i++) {
        if (isPrime(thread_args->numbers[i])) {
            thread_args->prime_count++;
        }
    }
    pthread_exit(NULL);
}

int main() {
    // The main function runs the proram and uses the optimized isPrime function to check if a number is prime.
    int num;
    int total_counter = 0;

    // Determine number of CPU cores
    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    // Fallback to 1 if detection fails
    if (num_cores <= 0) num_cores = 1;
    // Use number of cores as number of threads for efficiency - we found it to be the most efficient
    const int NUM_THREADS = num_cores;

    // Allocate memory for chunk of numbers to process
    int *numbers = (int *)malloc(CHUNK_SIZE * sizeof(int));
    if (numbers == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Use buffered input for faster reading - read 1024 bytes at a time using the setvbuf system call
    char buffer[1024];
    setvbuf(stdin, buffer, _IOFBF, sizeof(buffer));

    while (true) {
        // Read and process numbers in chunks until EOF
        int num_count = 0;

        // Read a chunk of numbers from input while not reaching EOF and not exceeding CHUNK_SIZE
        while (num_count < CHUNK_SIZE && scanf("%d", &num) != EOF) {
            numbers[num_count++] = num;
        }

        // No more numbers to process - break
        if (num_count == 0) break;

        // Create threads to count primes in parallel for each chunk of numbers
        pthread_t threads[NUM_THREADS];
        // Array to hold thread arguments for each thread
        PrimeThreadArgs thread_args[NUM_THREADS];
        // The range of numbers each thread will process
        int range = num_count / NUM_THREADS;

        for (int i = 0; i < NUM_THREADS; i++) {
            // Initt thread arguments with chunk of numbers and a range of numbers to process
            thread_args[i].numbers = numbers;
            thread_args[i].start = i * range;
            thread_args[i].end = (i == NUM_THREADS - 1) ? num_count : (i + 1) * range;

            // Create thread to count primes using the countPrimes assistance function
            if (pthread_create(&threads[i], NULL, countPrimes, (void *)&thread_args[i]) != 0) {
                fprintf(stderr, "Error creating thread\n");
                free(numbers);
                return 1;
            }
        }

        // Wait for threads to finish using the pthread_join system call
        for (int i = 0; i < NUM_THREADS; i++) {
            if (pthread_join(threads[i], NULL) != 0) {
                fprintf(stderr, "Error joining thread\n");
                free(numbers);
                return 1;
            }
            // Accumulate total prime count to output at the end like the original code
            total_counter += thread_args[i].prime_count;
        }

        // If reached the end of input, break
        if (num_count < CHUNK_SIZE) break;
    }

    // Output results like the original code to compare correctness
    printf("%d total primes.\n", total_counter);

    // Free daynamic memory allocated for numbers 
    free(numbers);
    return 0;
}
