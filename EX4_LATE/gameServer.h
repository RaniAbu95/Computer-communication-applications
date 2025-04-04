#ifndef GUESS_NUMBER_SERVER_H
#define GUESS_NUMBER_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <signal.h>
#include <errno.h>

#define MAX_BUFFER 256

// Global Variables
extern int server_socket;
extern int target_number;
extern int max_players;
extern fd_set master_read_set, master_write_set;
extern int client_sockets[];
extern char messages[][MAX_BUFFER];

// Function Declarations
void init_server(int port, int max_players);
void handle_new_connection();
void handle_player_guess(int player_id);
void remove_player(int player_id);
void broadcast_message(const char *message, int exclude_id);
void cleanup();
void signal_handler(int signo);

#endif // GUESS_NUMBER_SERVER_H
