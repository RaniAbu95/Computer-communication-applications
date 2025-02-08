//
// Created by Rani Abu Raia on 05/02/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <signal.h>

#define MAX_PLAYERS 10
#define BUFFER_SIZE 256

// Player structure
typedef struct {
    int socket;
    int id;
    char buffer[BUFFER_SIZE];
} Player;

// Global variables
int server_socket;
Player players[MAX_PLAYERS];
int num_players = 0;
int target_number;
fd_set read_fds, write_fds, master_fds;

void cleanup() {
    printf("Shutting down server...\n");
    for (int i = 0; i < num_players; i++) {
        if (players[i].socket > 0) {
            close(players[i].socket);
        }
    }
    close(server_socket);
    exit(0);
}

void handle_signal(int signo) {
    if (signo == SIGINT) {
        cleanup();
    }
}

void init_server(int port, int seed, int max_players) {
    struct sockaddr_in server_addr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket error");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(1);
    }

    if (listen(server_socket, max_players) < 0) {
        perror("Listen error");
        exit(1);
    }

    srand(seed);
    target_number = rand() % 100 + 1;
    printf("Server started on port %d, target number: %d\n", port, target_number);
}

void handle_new_connection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int new_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
    if (new_socket < 0) {
        perror("Accept error");
        return;
    }

    if (num_players >= MAX_PLAYERS) {
        printf("Max players reached, rejecting connection\n");
        close(new_socket);
        return;
    }

    players[num_players].socket = new_socket;
    players[num_players].id = num_players + 1;
    num_players++;
    FD_SET(new_socket, &master_fds);

    char welcome_msg[BUFFER_SIZE];
    sprintf(welcome_msg, "Welcome to the game, your id is %d\n", num_players);
    send(new_socket, welcome_msg, strlen(welcome_msg), 0);
    printf("New player joined with ID %d\n", num_players);
}

void handle_player_guess(int index) {
    int player_socket = players[index].socket;
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(player_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytes_read <= 0) {
        printf("Player %d disconnected\n", players[index].id);
        close(player_socket);
        FD_CLR(player_socket, &master_fds);
        players[index] = players[num_players - 1];
        num_players--;
        return;
    }

    buffer[bytes_read] = '\0';
    int guess = atoi(buffer);
    printf("Player %d guessed %d\n", players[index].id, guess);

    char response[BUFFER_SIZE];
    if (guess < target_number) {
        sprintf(response, "The guess %d is too low\n", guess);
    } else if (guess > target_number) {
        sprintf(response, "The guess %d is too high\n", guess);
    } else {
        sprintf(response, "Player %d wins! The correct number is %d\n", players[index].id, target_number);
        for (int i = 0; i < num_players; i++) {
            send(players[i].socket, response, strlen(response), 0);
            close(players[i].socket);
        }
        num_players = 0;
        target_number = rand() % 100 + 1;
        printf("Game reset with new target number: %d\n", target_number);
    }
    send(player_socket, response, strlen(response), 0);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: ./server <port> <seed> <max-number-of-players>\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    int seed = atoi(argv[2]);
    int max_players = atoi(argv[3]);

    if (port <= 0 || port >= 65536 || max_players < 2) {
        fprintf(stderr, "Invalid arguments\n");
        exit(1);
    }

    signal(SIGINT, handle_signal);
    init_server(port, seed, max_players);

    FD_ZERO(&master_fds);
    FD_SET(server_socket, &master_fds);

    while (1) {
        read_fds = master_fds;
        if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Select error");
            exit(1);
        }

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == server_socket) {
                    handle_new_connection();
                } else {
                    for (int j = 0; j < num_players; j++) {
                        if (players[j].socket == i) {
                            handle_player_guess(j);
                            break;
                        }
                    }
                }
            }
        }
    }
    return 0;
}
