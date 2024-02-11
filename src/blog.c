#include "blog.h"

// Read a file (or a socket) safely** (after proper error handling is implemented)
// Can optimize to avoid unnecessary realloc
// reference: https://stackoverflow.com/a/44894946
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
