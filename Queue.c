#include "Queue.h"

pQueue createQueue()
{
    pQueue newQueue = (pQueue)malloc(sizeof(Queue));
    if (newQueue == NULL)
    {
        perror("malloc");
        return NULL;
    }
    newQueue->size = 0;
    newQueue->first = NULL;
    newQueue->last = NULL;
    pthread_cond_init(&newQueue->cond, NULL);
    pthread_mutex_init(&newQueue->lock, NULL);
    return newQueue;
}

void freeQueue(pQueue queue)
{
    pthread_mutex_lock(&queue->lock);
    pNode to_free;
    while (queue->first != NULL)
    {
        to_free = queue->first;
        queue->first = to_free->next;
        free(to_free->data);
        free(to_free);
    }
    pthread_cond_destroy(&queue->cond);
    pthread_mutex_destroy(&queue->lock);
    pthread_mutex_unlock(&queue->lock);
    free(queue);
}

int isEmpty(pQueue queue)
{
    if(queue == NULL){ return 0; }
    return queue->size == 0;
}

void enqueue(pQueue queue, void *data)
{
    pthread_mutex_lock(&queue->lock);
    pNode node = (pNode)malloc(sizeof(Node));
    if (node == NULL)
    {
        perror("malloc");
        exit(1);
    }
    node->data = data;
    node->next = NULL;
    // Insert the first element
    if (queue->size == 0)
    {
        queue->first = node;
        queue->last = node;
    }
    else
    {
        queue->last->next = node;
        queue->last = node;
    }
    queue->size++;

    // Signal to the thread who want to dequeue the queue is not empty anymore
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
}

void *dequeue(pQueue queue)
{
    pthread_mutex_lock(&queue->lock);
    // The while loop is because the thread can wake up while another thread already consume the queue,
    // So he needs to wait again to the queue to be fill
    while (isEmpty(queue))
    {
        // Wait for someone to signal the queue is not empty
        pthread_cond_wait(&queue->cond, &queue->lock);
    }
    pNode to_delete = queue->first;
    queue->first = to_delete->next;
    void *data = to_delete->data;
    free(to_delete);
    queue->size--;

    pthread_mutex_unlock(&queue->lock);
    return data;
}