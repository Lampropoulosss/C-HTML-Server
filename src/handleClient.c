#include "handleClient.h"
#include "getMimeType.h"

#include <stddef.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define BUFF_SIZE 1024
#define PATH_SIZE 512
#define PUBLIC_DIR "./public"

#define BAD_REQUEST "HTTP/1.1 400 Bad Request\r\n\r\n"
#define NOT_FOUND "HTTP/1.1 404 Not Found\r\n\r\n"
#define FORBIDDEN "HTTP/1.1 403 Forbidden\r\n\r\n"
#define SERVER_ERROR "HTTP/1.1 500 Internal Server Error\r\n\r\n"

int handleClient(int client_fd)
{
    char buffer[BUFF_SIZE] = {0};

    if (recv(client_fd, buffer, BUFF_SIZE - 1, 0) < 0)
    {
        perror("Could not receive request.\n");
        printf("===================================\n");
        return EXIT_FAILURE;
    }

    char method[8] = {0};
    char path[PATH_SIZE + 1 + 10] = {0}; // +1 for null terminator and + 10 for file extension

    sscanf(buffer, "%7s %512s", method, path);

    // Append extension if ommited or append index.html
    if (strcmp(path, "/") == 0)
    {
        strcpy(path, "/index.html");
    }
    else if (strchr(path, '.') == NULL)
    {
        strcat(path, ".html");
    }

    // Currently only supporting GET requests.
    if (strcmp(method, "GET") != 0)
    {
        fprintf(stderr, "Unsupported method: %s\n", method);
        printf("===================================\n");
        send(client_fd, BAD_REQUEST, strlen(BAD_REQUEST), 0);
        return EXIT_FAILURE;
    }

    char *absolute_public_path = realpath(PUBLIC_DIR, NULL);
    if (!absolute_public_path)
    {
        perror("Could not get the absolute path of public directory.\n");
        printf("===================================\n");
        send(client_fd, SERVER_ERROR, strlen(SERVER_ERROR), 0);
        return EXIT_FAILURE;
    }

    size_t full_path_len = strlen(absolute_public_path) + strlen(path) + 1;

    char full_path[full_path_len];

    snprintf(full_path, full_path_len, "%s%s", absolute_public_path, path);

    char *resolved_full_path = realpath(full_path, NULL);
    if (!resolved_full_path)
    {
        fprintf(stderr, "Could not resolve requested file path.\nPath: %s\n", full_path);
        printf("===================================\n");
        send(client_fd, NOT_FOUND, strlen(NOT_FOUND), 0);
        free(absolute_public_path);
        return EXIT_FAILURE;
    }

    size_t absolute_public_path_len = strlen(absolute_public_path);

    if (strncmp(absolute_public_path, resolved_full_path, absolute_public_path_len) != 0 || (resolved_full_path[absolute_public_path_len] != '/' && resolved_full_path[absolute_public_path_len] != '\0'))
    {
        fprintf(stderr, "Security violation: attempted directory traversal.\nPath: %s\n", resolved_full_path);
        printf("===================================\n");
        send(client_fd, FORBIDDEN, strlen(FORBIDDEN), 0);
        free(absolute_public_path);
        return EXIT_FAILURE;
    }

    free(absolute_public_path);

    FILE *file;

    if ((file = fopen(resolved_full_path, "rb")) == NULL)
    {
        perror("Could not open file.\n");
        printf("===================================\n");
        send(client_fd, SERVER_ERROR, strlen(SERVER_ERROR), 0);
        return EXIT_FAILURE;
    }

    free(resolved_full_path);

    char file_buffer[BUFF_SIZE] = {0};
    ssize_t bytes_read;

    // temporarily use file_buffer for the headers
    int header_length = snprintf(file_buffer, BUFF_SIZE,
                                 "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: %s\r\n"
                                 "Connection: keep-alive\r\n"
                                 "\r\n",
                                 get_mime_type(path));

    if (send(client_fd, file_buffer, header_length, 0) < 0)
    {
        perror("Could not send headers.\n");
        printf("===================================\n");
        send(client_fd, SERVER_ERROR, strlen(SERVER_ERROR), 0);
        fclose(file);
        return EXIT_FAILURE;
    }

    while ((bytes_read = fread(file_buffer, 1, BUFF_SIZE, file)) > 0)
    {
        if (send(client_fd, file_buffer, bytes_read, 0) < 0)
        {
            perror("Could not send file.\n");
            printf("===================================\n");
            send(client_fd, SERVER_ERROR, strlen(SERVER_ERROR), 0);
            fclose(file);
            return EXIT_FAILURE;
        }
    }

    fclose(file);

    printf("Request received:\n\n");
    // printf("%s\n", buffer);
    printf("Method: %s\nPath: %s\nFull path: %s\n", method, path, full_path);
    printf("===================================\n");

    return EXIT_SUCCESS;
}