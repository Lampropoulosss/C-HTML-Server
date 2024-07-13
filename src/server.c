#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "handleClient.h"

#define PORT 8080

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

int main()
{
    int server_fd;
    sockaddr_in server_addr;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket Creation Failed.");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Socket Binding Failed.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 20) < 0)
    {
        perror("Socket Listening Failed.");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof(client_addr);
        int *client_fd;

        if ((client_fd = malloc(sizeof(int))) == NULL)
        {
            perror("Could not allocate memory.\n");
            continue;
        }

        if ((*client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_addr_length)) < 0)
        {
            perror("Could not accept connection.\n");
            free(client_fd);
            continue;
        }

        pthread_t thread_id;

        pthread_create(&thread_id, NULL, handleClient, (void *)client_fd);
        pthread_detach(thread_id);
    }

    return 0;
}