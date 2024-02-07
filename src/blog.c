#include "blog.h"

// Read a file (or a socket) safely** (after proper error handling is implemented)
// Can optimize to avoid unnecessary realloc
// reference: https://stackoverflow.com/a/44894946
int read_file(FILE *fp, char **data_ptr, size_t *size_ptr) {
    if (fp == NULL || data_ptr == NULL || size_ptr == NULL) {
        return FILE_READ_INVALID;
    }

    char *file_data = NULL;

    size_t total_read = 0;
    size_t buffer_size = 0;
    size_t temp = 0;

    while (true) {
        if (READ_CHUNK_SIZE + total_read + 1 > buffer_size) {
            // Buffer too small
            // Reallocate additional READ_CHUNK_SIZE + 1 bytes
            buffer_size += READ_CHUNK_SIZE + 1;
            file_data = (char *) realloc(file_data, buffer_size);
        }

        temp = fread(file_data + total_read, 1, READ_CHUNK_SIZE, fp);
        if (temp == 0) {
            // Nothing was read
            // We have reached EOF or something went wrong
            break;
        }

        total_read += temp;
    }

    // Reallocate, null-terminate, assign
    file_data = (char *) realloc(file_data, total_read + 1);
    file_data[total_read] = '\0';
    *data_ptr = file_data;

    *size_ptr = total_read;

    return FILE_READ_OK;
}

// Add 200 header (with newlines) to content
char *add_200_header(char *body) {
    char *header = (char *) malloc(HTTP_200_HEADER_LENGTH + 1);
    strcpy(header, HTTP_200_HEADER);

    // Append body to header
    size_t body_len = strlen(body);
    char *output = (char *) malloc(HTTP_200_HEADER_LENGTH + body_len + 1);
    strcpy(output, header);
    strcat(output, body);
    
    // Free allocated memory
    free(header);
    
    return output;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s <file_path>\n", argv[0]);
        exit(0);
    }

    // Read content from file into file_data
    char *file_data;
    size_t data_length;

    FILE *fp = fopen(argv[1], "r");
    if (read_file(fp, &file_data, &data_length) != FILE_READ_OK) {
        fprintf(stderr, "error reading file");
        exit(1);
    }
    fclose(fp);

    // Append header to file
    file_data = add_200_header(file_data);

    int status;
    struct addrinfo hints;
    struct addrinfo *my_info;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, PORT, &hints, &my_info)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // Get a socket and bind port to it
    int sock_fd = socket(my_info->ai_family, my_info->ai_socktype, my_info->ai_protocol);
    bind(sock_fd, my_info->ai_addr, my_info->ai_addrlen);

    // Listen for connections
    listen(sock_fd, BACKLOG);

    // Accept connections, fork, print, close
    for (unsigned int i = 0; i >= 0; i++) {
        // Serve one connection
        struct sockaddr_storage incoming_addr;
        socklen_t sin_size = sizeof incoming_addr;

        int new_fd = accept(sock_fd, (struct sockaddr *)&incoming_addr, &sin_size);

        if (new_fd < 0) {
            perror("accept");
            exit(1);
        }

        if (!fork()) {
            // Child process
            close(sock_fd);

            int bytes_sent = send(new_fd, file_data, data_length, 0);
            if (bytes_sent <= 0) {
                perror("send");
                exit(1);
            } else {
                printf("Successfully sent\n");
            }

            close(new_fd);
            exit(0);
        }
        
        close(new_fd);
    }

    return 0;
}
