//#include <stdio.h>
//#include <stdbool.h>
//#include <time.h>
//
//#include <stdlib.h>
//#include <sys/resource.h>
//
//
//
//
//// Function to check if a number is prime
//bool isPrime(int n) {
//    if (n <= 1) {
//        return false;
//    }
//    for (int i = 2; i * i <= n; i++) {
//        if (n % i == 0) {
//            return false;
//        }
//    }
//    return true;
//}
//
//int main() {
//    int num;
//    int total_counter = 0;
//
////    struct rlimit rl;
////    rl.rlim_cur = 2 * 1024 * 1024; // Set a 2MB limit
////    rl.rlim_max = 2 * 1024 * 1024; // Set a 2MB hard limit
////    if(setrlimit(RLIMIT_AS, &rl) != 0)
////    {
////        perror("setrlimit");
////        exit(2);
////    }
//
//    // Read numbers from stdin until end of file
//    while (scanf("%d", &num) != EOF) {
//        if (isPrime(num)) {
//            total_counter++;
//        }
//    }
//
//    printf("%d total primes.\n", total_counter);
//
//    return 0;
//}

// ###############################################################################
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "Queue.h"

#define NUM_THREADS 8  // Adjust based on your system capabilities

pQueue global_queue;


size_t stacksize = 512 * 1024; // 512 KB stack size per thread


// Function to check if a number is prime
bool isPrime(int n)
{
    if (n <= 1)return false;
    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0) return false;
    }
    return true;
}

typedef struct{
    int count;
    pthread_mutex_t lock;
} PrimeData;

void *processNumbers(void *arg)
{
    PrimeData *primeData = (PrimeData *) arg;
    int localCount = 0;
    int *numPtr;

    // Use the global queue passed to the threads
    while ((numPtr = dequeue(global_queue)) != NULL)
    {  // Ensure global_queue is accessible here
        if (isPrime(*numPtr))
        {
            localCount++;
        }
        free(numPtr);
    }

    pthread_mutex_lock(&primeData->lock);
    primeData->count += localCount;
    pthread_mutex_unlock(&primeData->lock);

    return NULL;
}

pQueue global_queue;  // Declare the queue globally

int main()
{
    pthread_t threads[NUM_THREADS];
    PrimeData primeData = {0, PTHREAD_MUTEX_INITIALIZER};

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, stacksize);

    global_queue = createQueue();  // Initialize the queue only once

    // Start threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&threads[i], NULL, processNumbers, &primeData))
//        if(pthread_create(&threads[i], &attr, processNumbers, &primeData) )
        {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Read numbers from stdin and enqueue them
    int num;
    while (scanf("%d", &num) != EOF)
    {
        int *numPtr = malloc(sizeof(int));
        *numPtr = num;
        enqueue(global_queue, numPtr);
    }

    // Signal threads to stop
    for (int i = 0; i < NUM_THREADS; i++)
    {
        enqueue(global_queue, NULL);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("%d total primes.\n", primeData.count);
    freeQueue(global_queue);
    pthread_attr_destroy(&attr); // Don't forget to destroy the attr object

    return 0;
}
