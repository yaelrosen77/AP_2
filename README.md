# AP_2
Primmality check optimization through multi-threading

## Task Mission
The goal of this assignment is to count the number of prime numbers in a simulated endless stream of random numbers. The task involves several key constraints and requirements:

* Random Number Stream: Use a provided random number generator to simulate an endless stream of numbers.
* Prime Counting: Update the given program to count the number of prime numbers within this stream.
* Parallelization: Utilize all available CPU cores to optimize performance through parallel processing.
* Memory Constraint: Limit the program's memory usage to a maximum of 2MB.
* Efficiency: Ensure that the program is optimized for both performance and memory usage, potentially improving the isPrime function as needed.

## Solution Implementation
Prime Number Check Function: The isPrime function is optimized to efficiently determine if a number is prime by reducing the number of divisors it checks against:
* Base cases - we check for a given 1 2 or 3 and anwear accordignaly
* Exclusion of Even Numbers and Multiples of 3 - best for exclued immeditaly without checking all divisors which is essential for improving run time
* Check Only Necessary Numbers- Instead of checking all numbers, the function checks numbers of the form 6k ± 1 (i.e., i and i + 2 in the loop). This is based on the observation that all prime numbers greater than 3 can be written in the form 6k ± 1. This skips unnecessary checks, making the loop more efficient.

### Memory Management:
* Batch Processing: The program reads numbers in chunks (batches) to avoid excessive memory usage. Each batch is processed before reading the next set of numbers.
* Memory Allocation: Carefully manage memory allocation and reallocation within the 2MB limit.
* Parallel Processing:
  * Thread Pool: Utilize a pool of threads equal to the number of CPU cores to divide the workload.
  * Thread Arguments: Pass the necessary data to each thread, ensuring that they work on distinct portions of the number stream to avoid overlap and contention.
  * Synchronization: Use a lock-free queue to handle the communication between the main thread and worker threads efficiently, minimizing bottlenecks and contention.

## Results 
* We have managed to improve run-time approximatly ten times better than provided code.
* We had also managed to maintain memory constraints, even by running 1,000,000 range of numbers we stayed bellow 2MB

The following results represents running the *original code* on 10M of numbers range
![image](https://github.com/yaelrosen77/AP_2/assets/118935583/8d3df42f-5c37-43a3-a7c4-29deff123fc5)

The following results represents running the *improved code* on 10M of numbers range
![image](https://github.com/yaelrosen77/AP_2/assets/118935583/056914e9-37f5-4ee6-bf73-84c52c2a8758)

> [!NOTE]
> We can tell that the updated code time is approximately 10.15 times better than the original messurment time.
> We can also notice that memory consumptaion maintained bellow than 2MB (about 1.875 MB).

## How to run
1. Download the files
2. Compile the program with the instruction:
   ```make all```
3. Run
   ```randomGenerator <seed> <num_of numbers>``` where seed is the initial seed value for the random number generator and num_of_numbers is the number of random numbers to generate. The randomGenerator program will produce an endless stream of random numbers based on the provided seed value.

## Authors
* [Itamar Kuznitsov](https://github.com/Itamar-Kuznitsov)
* [Yael Rosen](https://github.com/yaelrosen77)

