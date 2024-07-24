#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

#include "taskQueue.h"
#include "worker_thread.h"

#define PORT 8080
#define MAX_CLIENTS 20

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;
typedef struct pollfd pollfd;

// Shared data
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pollfd poll_fds[MAX_CLIENTS + 1]; // +1 for the server_fd
unsigned int nfds = 0;            // Number of file descriptors in poll_fds
int server_fd;                    // Server socket file descriptor

Queue task_queue;

int main()
{
    sockaddr_in server_addr;

    initQueue(&task_queue);
    init_threads();

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket Creation Failed.");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Socket Binding Failed.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 20) < 0)
    {
        perror("Socket Listening Failed.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Initialize poll_fds
    poll_fds[0].fd = server_fd;
    poll_fds[0].events = POLLIN;
    nfds++;

    while (1)
    {
        pthread_mutex_lock(&mutex);

        printf("Waiting for connections...\n");

        int poll_count = poll(poll_fds, nfds, -1);
        if (poll_count < 0)
        {
            perror("Poll failed.");
            pthread_mutex_unlock(&mutex);
            continue;
        }

        for (unsigned int i = 0; i < nfds; i++)
        {
            if (poll_fds[i].revents & POLLIN)
            {
                if (poll_fds[i].fd == server_fd)
                {
                    // New connection
                    sockaddr_in client_addr;
                    socklen_t client_addr_length = sizeof(client_addr);
                    int client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_addr_length);
                    if (client_fd < 0)
                    {
                        perror("Could not accept connection.");
                        continue;
                    }

                    printf("Connection accepted.\n");

                    if (nfds < MAX_CLIENTS + 1)
                    {
                        poll_fds[nfds].fd = client_fd;
                        poll_fds[nfds].events = POLLIN;
                        nfds++;
                    }
                    else
                    {
                        printf("Max amount of clients reached. Closing new connection...\n");
                        close(client_fd);
                    }
                }
                else
                {
                    // Handle existing connection
                    int client_fd = poll_fds[i].fd;
                    poll_fds[i] = poll_fds[--nfds];
                    i--;

                    if (enqueue(&task_queue, client_fd) == EXIT_FAILURE)
                    {
                        close(client_fd);
                    }
                }
            }
        }

        pthread_mutex_unlock(&mutex);
    }

    close(server_fd);
    return 0;
}
