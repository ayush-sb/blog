#include "blog.h"

#define HTTP_200_HEADER "HTTP/1.1 200 OK"
#define HTTP_200_HEADER_LENGTH 15

// HTTP headers
char *add_200_header(char *body) {
    char *header = (char *) malloc((HTTP_200_HEADER_LENGTH + 1) * sizeof(char));
    strcpy(header, HTTP_200_HEADER);

    // Append body to header
    size_t body_len = strlen(body);
    char *output = (char *) malloc((HTTP_200_HEADER_LENGTH + body_len + 1) * sizeof(char));
    strcpy(output, header);
    strcat(output, body);
    
    // Free memory
    free(header);
    return output;
}

int main(int argc, char **argv) {
    // Five messages to prevent caching
    char *messages[] = {
        "\n\n<html><body><p>abcdef ghij klmn</p></body></html>",
        "\n\n<html><body><p>normal text here</p></body></html>",
        "\n\n<html><body><p>helpp im getting</p></body></html>",
        "\n\n<html><body><p>123456 7890123 4</p></body></html>",
        "\n\n<html><body><p>lorem ipsum dolo</p></body></html>"
    };

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

            char *message = add_200_header(messages[i % 5]);
            int bytes_sent = send(new_fd, message, strlen(message), 0);

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
