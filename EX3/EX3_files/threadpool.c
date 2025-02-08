//
// Created by Rani Abu Raia on 18/01/2025.
//

#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// Define limits for the parameters
#define MAX_THREADS 64
#define MAX_QUEUE_SIZE 1024

threadpool* create_threadpool(int num_threads_in_pool, int max_queue_size){
    // Check limits of parameters
    if (num_threads_in_pool <= 0 || num_threads_in_pool > MAX_THREADS ||
    max_queue_size <= 0 || max_queue_size > MAX_QUEUE_SIZE) {
        fprintf(stderr, "Invalid threadpool parameters.\n");
        return NULL;
    }

    // Allocate memory for the threadpool
    threadpool *pool = (threadpool *)malloc(sizeof(threadpool));
    if (!pool) {
        perror("Failed to allocate memory for threadpool");
        return NULL;
    }

    // Initialize threadpool structure
    pool->num_threads = num_threads_in_pool;
    pool->qsize = 0;
    pool->max_qsize = max_queue_size;
    pool->threads = (pthread_t *)malloc(num_threads_in_pool * sizeof(pthread_t));
    pool->qhead = NULL;
    pool->qtail = NULL;
    pool->shutdown = 0;
    pool->dont_accept = 0;

    // Check if memory allocation for threads succeeded
    if (!pool->threads) {
        perror("Failed to allocate memory for threads");
        free(pool);
        return NULL;
    }

    // Initialize mutex and condition variables
    if (pthread_mutex_init(&pool->qlock, NULL) != 0 ||
    pthread_cond_init(&pool->q_not_empty, NULL) != 0 ||
    pthread_cond_init(&pool->q_empty, NULL) != 0 ||
    pthread_cond_init(&pool->q_not_full, NULL) != 0) {
        perror("Failed to initialize mutex or condition variables");
        free(pool->threads);
        free(pool);
        return NULL;
    }

    // Create threads
    for (int i = 0; i < num_threads_in_pool; i++) {
        if (pthread_create(&pool->threads[i], NULL, do_work, (void *)pool) != 0) {
            perror("Failed to create threads");
            // Clean up if thread creation fails
            for (int j = 0; j < i; j++) {
                pthread_cancel(pool->threads[j]);
            }
            free(pool->threads);
            pthread_mutex_destroy(&pool->qlock);
            pthread_cond_destroy(&pool->q_not_empty);
            pthread_cond_destroy(&pool->q_empty);
            pthread_cond_destroy(&pool->q_not_full);
            free(pool);
            return NULL;
        }
    }

    return pool;
}



void dispatch(threadpool* from_me, dispatch_fn dispatch_to_here, void *arg){

}


void* do_work(void* p){
    threadpool* pool = (threadpool*)p;
    while (1) {
        pthread_mutex_lock(&pool->qlock);

        // Step 1: Check if the destruction process has begun
        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->qlock);
            pthread_exit(NULL);
        }

        // Step 2: If the queue is empty, wait for a job
        while (pool->qsize == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->q_not_empty, &pool->qlock);
        }

        // Step 3: Check the destruction flag again after waking up
        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->qlock);
            pthread_exit(NULL);
        }

        // Step 4: Take the first element from the queue
        work_t* work = pool->qhead;
        if (work != NULL) {
            pool->qhead = work->next;
            pool->qsize--;

            // Step 5: If the queue becomes empty and destruction process is waiting to begin
            if (pool->qsize == 0 && pool->dont_accept) {
                pthread_cond_signal(&pool->q_empty);
            }

            // Step 6: If destruction hasn't begun, signal there is free space in the queue
            if (!pool->dont_accept) {
                pthread_cond_signal(&pool->q_not_full);
            }
        }

        pthread_mutex_unlock(&pool->qlock);

        // Step 7: Call the thread routine
        if (work != NULL) {
            work->routine(work->arg);
            free(work); // Free the work after execution
        }
    }

}

void destroy_threadpool(threadpool* destroyme) {
    if (destroyme == NULL) {
        return;
    }

    // Step 1: Set don't_accept flag to 1
    pthread_mutex_lock(&destroyme->qlock);
    destroyme->dont_accept = 1;

    // Step 2: Wait for the queue to become empty
    while (destroyme->qsize > 0) {
        pthread_cond_wait(&destroyme->q_empty, &destroyme->qlock);
    }

    // Step 3: Set shutdown flag to 1
    destroyme->shutdown = 1;

    // Step 4: Signal threads waiting on the empty queue
    pthread_cond_broadcast(&destroyme->q_not_empty);
    pthread_mutex_unlock(&destroyme->qlock);

    // Step 5: Join all threads
    for (int i = 0; i < destroyme->num_threads; i++) {
        pthread_join(destroyme->threads[i], NULL);
    }

    // Step 6: Free resources
    free(destroyme->threads);

    pthread_mutex_destroy(&destroyme->qlock);
    pthread_cond_destroy(&destroyme->q_not_empty);
    pthread_cond_destroy(&destroyme->q_empty);
    pthread_cond_destroy(&destroyme->q_not_full);

    free(destroyme);
}

int enqueue_work(threadpool* pool, work_t* work) {
    pthread_mutex_lock(&pool->qlock);

    // Check if the pool is not accepting work
    if (pool->dont_accept) {
        pthread_mutex_unlock(&pool->qlock);
        return -1;
    }

    // Check if the queue is full
    while (pool->qsize >= pool->max_qsize) {
        pthread_cond_wait(&pool->q_not_full, &pool->qlock);
    }

    // Add the work to the queue
    if (pool->qtail) {
        pool->qtail->next = work;
    } else {
        pool->qhead = work;
    }
    pool->qtail = work;
    work->next = NULL;
    pool->qsize++;

    // Signal that the queue is not empty
    pthread_cond_signal(&pool->q_not_empty);
    pthread_mutex_unlock(&pool->qlock);

    return 0;
}