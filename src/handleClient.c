#include "handleClient.h"

#define BUFF_SIZE 1024

int handleClient(int client_fd)
{
    char buffer[BUFF_SIZE] = {0};

    if (recv(client_fd, buffer, BUFF_SIZE - 1, 0) < 0)
    {
        perror("Could not receive message.\n");
        return EXIT_FAILURE;
    }

    printf("%s", buffer);

    return EXIT_SUCCESS;
}