#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server-address> <server-port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* server_address = argv[1];
    int server_port = atoi(argv[2]);

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Server address setup
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);


    if (inet_pton(AF_INET, server_address, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (!connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    // Send request
    const char* request = "GET abc/ HTTP/1.0\n";
    send(sock, request, strlen(request), 0);
    printf("Request sent:\n%s", request);

    // Receive response
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = read(sock, buffer, sizeof(buffer) - 1);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Response received:\n%s\n", buffer);
    } else {
        perror("Read error");
    }

    // Close connection
    close(sock);
    return 0;
}
