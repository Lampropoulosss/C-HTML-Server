#include "handleClient.h"

#include <stddef.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 1024
#define PATH_SIZE 512
#define PUBLIC_DIR "./public"

int handleClient(int client_fd)
{
    char buffer[BUFF_SIZE] = {0};

    if (recv(client_fd, buffer, BUFF_SIZE - 1, 0) < 0)
    {
        perror("Could not receive message.\n");
        return EXIT_FAILURE;
    }

    char method[8] = {0};
    char path[PATH_SIZE + 1] = {0}; // +1 for null terminator

    sscanf(buffer, "%7s %512s", method, path);

    // Currently only supporting GET requests.
    if (strcmp(method, "GET") != 0)
    {
        fprintf(stderr, "Unsupported method: %s\n", method);
        return EXIT_FAILURE;
    }

    char *absolute_public_path = realpath(PUBLIC_DIR, NULL);
    if (!absolute_public_path)
    {
        perror("Could not get absolute of public directory.\n");
        return EXIT_FAILURE;
    }

    size_t full_path_len = strlen(absolute_public_path) + strlen(path) + 1;

    char full_path[full_path_len];
    full_path[full_path_len - 1] = '\0';

    snprintf(full_path, full_path_len, "%s%s", absolute_public_path, path);

    char resolved_full_path[full_path_len];
    if (!realpath(full_path, resolved_full_path))
    {
        fprintf(stderr, "Could not resolve requested file path.\n");
        free(absolute_public_path);
        return EXIT_FAILURE;
    }

    size_t absolute_public_path_len = strlen(absolute_public_path);

    if (strncmp(absolute_public_path, resolved_full_path, absolute_public_path_len) != 0 || (resolved_full_path[absolute_public_path_len] != '/' && resolved_full_path[absolute_public_path_len] != '\0'))
    {
        fprintf(stderr, "Security violation: attempted directory traversal.\n");
        free(absolute_public_path);
        return EXIT_FAILURE;
    }

    free(absolute_public_path);

    printf("%s\n", buffer);
    printf("Method: %s\n", method);
    printf("Path: %s\n", path);
    printf("size: %ld\n", full_path_len);
    printf("Full path: %s\n", full_path);

    return EXIT_SUCCESS;
}