#include "worker_thread.h"

#define WORKER_THREADS 2

extern Queue task_queue; // Initialized in server.c

pthread_t thread_ids[WORKER_THREADS];

void init_threads()
{
    for (unsigned short int i = 0; i < WORKER_THREADS; i++)
    {
        pthread_create(&thread_ids[i], NULL, worker_thread, NULL);
    }
}

void *worker_thread()
{
    while (1)
    {
        int client_fd = dequeue(&task_queue);

        if (handleClient(client_fd) == EXIT_FAILURE)
        {
            close(client_fd);
        }

        close(client_fd);
    }

    return NULL;
}