#include "taskQueue.h"

#include <stdlib.h>
#include <stdio.h>

void initQueue(Queue *queue)
{
    queue->front = queue->rear = NULL;

    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
}

int enqueue(Queue *queue, int client_fd)
{
    Task *new_task;

    if ((new_task = (Task *)malloc(sizeof(Task))) == NULL)
    {
        perror("Could not allocate memory for new task.\n");
        return EXIT_FAILURE;
    }

    new_task->client_fd = client_fd;
    new_task->next = NULL;

    pthread_mutex_lock(&queue->mutex);

    if (queue->rear == NULL)
    {
        queue->front = queue->rear = new_task;
    }
    else
    {
        queue->rear->next = new_task;
        queue->rear = new_task;
    }

    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);

    return EXIT_SUCCESS;
}

int dequeue(Queue *queue)
{
    pthread_mutex_lock(&queue->mutex);

    while (queue->front == NULL)
    {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    Task *task = queue->front;
    queue->front = queue->front->next;

    int client_fd = task->client_fd;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(task);

    pthread_mutex_unlock(&queue->mutex);

    return client_fd;
}