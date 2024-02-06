#include "blog.h"

char *html = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 16\n\n<h1>testing</h1>";
// char *response = "HTTP/1.1 200 OK\r\nDate: %s\r\nContent-Type: %s\r\n\n";

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
	while (true) {
		struct sockaddr_storage incoming_addr;
		socklen_t sin_size = sizeof incoming_addr;

		int new_fd = accept(sock_fd, (struct sockaddr *)&incoming_addr, &sin_size);

		if (new_fd < 0) {
			fprintf(stderr, "error: accept\n");
			continue;
		}

		if (!fork()) {
			// Child process
			close(sock_fd);

			/*
			char buf[MSG_LEN];
			int bytes_recv = recv(new_fd, buf, MSG_LEN, 0);

			if (bytes_recv > 0) {
				buf[bytes_recv] = '\0';
				printf("%s", buf);
			}
			*/

			size_t html_len = (sizeof html) / (sizeof html[0]);
			int bytes_sent = send(new_fd, html, html_len, 0);

			if (bytes_sent == 0) {
				fprintf(stderr, "error: send\n");
				exit(0);
			}

			printf("%s\n", html);

			close(new_fd);
			exit(0);
		}

		close(new_fd);
	}

	return 0;
}
