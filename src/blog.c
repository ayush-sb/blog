#include "blog.h"

int main(int argc, char **argv) {
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
    for (int i = 0; i < 9999; i++) {
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

            // Five messages to prevent caching
            char *messages[] = {
                "http/1.1 200 ok\n\n<html><body><p>abcdef ghij klmn</p></body></html>",
                "http/1.1 200 ok\n\n<html><body><p>normal text here</p></body></html>",
                "http/1.1 200 ok\n\n<html><body><p>helpp im getting</p></body></html>",
                "http/1.1 200 ok\n\n<html><body><p>123456 7890123 4</p></body></html>",
                "http/1.1 200 ok\n\n<html><body><p>lorem ipsum dolo</p></body></html>"
            };

            int html_len = strlen(messages[0]);
            int bytes_sent = send(new_fd, messages[i % 5], html_len, 0);

            if (bytes_sent <= 0) {
                perror("send");
                exit(1);
            } else {
                printf("Successfully sent\n");
            }

            close(new_fd);
        }
        
        close(new_fd);
    }

    return 0;
}
