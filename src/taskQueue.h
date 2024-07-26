#pragma once

#include <pthread.h>

typedef struct Task
{
    int client_fd;
    struct Task *next;
} Task;

typedef struct Queue
{
    Task *front;
    Task *rear;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue;

void initQueue(Queue *queue);
int enqueue(Queue *queue, int client_fd);
int dequeue(Queue *queue);