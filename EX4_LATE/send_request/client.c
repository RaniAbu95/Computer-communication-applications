//
// Created by Rani Abu Raia on 05/02/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFFER_SIZE 256

int client_socket;

void cleanup1() {
    printf("Disconnecting from server...\n");
    close(client_socket);
    exit(0);
}

void handle_signal1(int signo) {
    if (signo == SIGINT) {
        cleanup1();
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ./client <server-ip> <port>\n");
        exit(1);
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    if (port <= 0 || port >= 65536) {
        fprintf(stderr, "Invalid port number\n");
        exit(1);
    }

    signal(SIGINT, handle_signal1);

    struct sockaddr_in server_addr;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket error");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    //printf("Connected to server. Start guessing numbers!\n");
    char buffer[BUFFER_SIZE];
    fd_set read_fds;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(client_socket, &read_fds);

        if (select(client_socket + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Select error");
            exit(1);
        }

        if (FD_ISSET(client_socket, &read_fds)) {
            int bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_read <= 0) {
                printf("Server disconnected.\n");
                cleanup1();
            }
            buffer[bytes_read] = '\0';
            printf("%s", buffer);
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
                cleanup1();
            }
            send(client_socket, buffer, strlen(buffer), 0);
        }
    }
}
