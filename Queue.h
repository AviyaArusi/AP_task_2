#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct _Node
{
    void *data;
    struct _Node *next;
} Node, *pNode;

typedef struct _Queue
{
    pNode first; // The first element inserted into the queue
    pNode last;  // The last element inserted into the queue
    size_t size;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} Queue, *pQueue;

pQueue createQueue();

void freeQueue(pQueue queue);

int isEmpty(pQueue queue);

void enqueue(pQueue queue, void *data);

void *dequeue(pQueue queue);