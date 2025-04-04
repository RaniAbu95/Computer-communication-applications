#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <signal.h>
#include <errno.h>

#define MAX_PLAYERS 15
#define BUFFER_SIZE 256
int available_ids[MAX_PLAYERS] = {0};

#define SERVER_SOCKET_ID  3


int find_last_available_index() {
    int last_index = -1;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (available_ids[i] == 1) {
            last_index = i;
        }
    }

    return last_index;
}

void print_array() {
    printf("Available IDs: ");
    for (int i = 0; i < MAX_PLAYERS; i++) {
        printf("%d ", available_ids[i]);
    }
    printf("\n");
}

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
fd_set master_fds, read_fds,write_fds;

void cleanup() {
    //printf("Shutting down server...\n");
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
    target_number = (int) random() % 100 + 1;
}


void notify_players(int new_socket, int new_player_id) {
    char message[BUFFER_SIZE];
    printf("Server is ready to write to player %d on socket %d\n",new_player_id,new_socket);
    //printf("server is ready to write to player %d on socket %d\n",new_player_id,players[new_player_id].socket);
    sprintf(message, "player %d joined the game\n", new_player_id);

    for (int i = 0; i < num_players; i++) { // Send to all except new player
        //sleep(2);

        if(players[i].id!=new_player_id)
        {

            send(players[i].socket, message, strlen(message), 0);
        }

    }
}

int get_next_available_id() {
    for (int i = 0; i <= MAX_PLAYERS; i++) {
        if (available_ids[i] == 0) { // Find the lowest available ID
            available_ids[i] = 1; // Mark as taken
            //print_array();
            return i;
        }
    }
    return -1; // No available ID
}


void send_guess_feedback_to_all_players(const char *message) {
    for (int i = 0; i < num_players; i++) {
        send(players[i].socket, message, strlen(message), 0);
    }
}


void handle_player_guess(int index) {
    int player_socket = players[index].socket;
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(player_socket, buffer, sizeof(buffer) - 1, 0);
    printf("server is ready to read from player %d on socket %d\n",index+1,player_socket);
    if (bytes_read <= 0) {
        //printf("player %d disconnected\n", players[index].id);

        // Mark ID as available
        //available_ids[players[index].id] = 0;

        // Notify other players
        char disconnect_msg[BUFFER_SIZE];
        sprintf(disconnect_msg, "player %d disconnected\n", players[index].id);
        //print_array();
        for (int i = 0; i <= find_last_available_index(); i++) {
            if (i != index && available_ids[i]!=0) {
                send(players[i].socket, disconnect_msg, strlen(disconnect_msg), 0);
            }
        }

        close(player_socket);
        FD_CLR(player_socket, &master_fds);

        // Remove player from the list
        available_ids[index] = 0;
        //print_array();
        //players[index] = players[num_players - 1]; // Move last player to this position
        num_players--;

        return;
    }

    buffer[bytes_read] = '\0';
    int guess = atoi(buffer);
    char res[BUFFER_SIZE];
    sprintf(res, "Player %d guessed %d\n", players[index].id, guess);

    for (int i = 0; i < num_players; i++) {
        send(players[i].socket, res, strlen(res), 0);
    }
    printf("Server is ready to write to player %d on socket %d\n",index+1,player_socket);
    char response[BUFFER_SIZE];

    // Send feedback to all players


    if (guess < target_number) {
        sprintf(response, "The guess %d is too low\n", guess);
        send_guess_feedback_to_all_players(response);
    } else if (guess > target_number) {
        sprintf(response, "The guess %d is too high\n", guess);
        send_guess_feedback_to_all_players(response);
    } else {
        // Player guessed correctly
        sprintf(response, "Player %d wins! The correct number is %d\n", players[index].id, target_number);
        for (int i = 0; i < num_players; i++) {
            send(players[i].socket, response, strlen(response), 0);
            close(players[i].socket);
        }
        num_players = 0;
        target_number = rand() % 100 + 1;
        printf("Game reset with new target number: %d\n", target_number);
    }
    printf("server is ready to write to player %d on socket %d\n",players[index].id, player_socket);
}


void handle_new_connection() {
//    for(int k=0;k<MAX_PLAYERS;k++)
//    {
//        if(available_ids[k]==1)
//        {
//            handle_player_guess(k);
//        }
//    }
    printf("Server is ready to read from welcome socket %d\n",server_socket);
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

    int new_id = get_next_available_id()+1;
    if (new_id == -1) {
        printf("No available player IDs\n");
        close(new_socket);
        return;
    }

//    players[num_players].socket = new_socket;
//    num_players++;
//    players[num_players].id = new_id;
    players[new_id-1].socket = new_socket;
    num_players++;
    players[new_id-1].id = new_id;
    FD_SET(new_socket, &master_fds);

    char welcome_msg[BUFFER_SIZE];
    sprintf(welcome_msg, "Welcome to the game, your id is %d\n", new_id);
    send(new_socket, welcome_msg, strlen(welcome_msg), 0);


    notify_players(new_socket,new_id); // Notify other players

}


int is_number(const char *str) {
    char *endptr;
    strtol(str, &endptr, 10);  // Try to convert the string to an integer
    return *endptr == '\0';     // If we reached the end of the string, it's a valid number
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: ./server <port> <seed> <max-number-of-players>\n");
        exit(1);
    }

    char* seed = argv[2];

    char *endptr=NULL;
    int port = atoi(argv[1]);

    if (port < 1 || port > 65535) {  // Typical port range check
        fprintf(stderr, "Usage: ./server <port> <seed> <max-number-of-players>\n");
        exit(1);
    }

    if (!is_number(seed)) {
        fprintf(stderr, "Usage: ./server <port> <seed> <max-number-of-players>\n");
        exit(1);
    }

    int seed_to_int = (int)strtol(argv[2], &endptr, 10);

    int max_players = (int) strtol(argv[3], &endptr, 10);

    if (max_players <= 0) {
        fprintf(stderr, "Usage: ./server <port> <seed> <max-number-of-players>\n");
        exit(1);
    }
    signal(SIGINT, handle_signal);
    init_server(port, seed_to_int, max_players);

    FD_ZERO(&master_fds);
    FD_SET(server_socket, &master_fds);
    struct timeval timeout;
    timeout.tv_sec = 5; // 5-second timeout
    timeout.tv_usec = 0;
    while (1) {
        read_fds = master_fds;
        write_fds = master_fds;
        if (select(FD_SETSIZE, &read_fds, &write_fds, NULL, &timeout) < 0) {
            perror("Select error");
            exit(1);
        }

// FIRST LOOP: Handle all player messages first
        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &read_fds) && i != server_socket) { // Ignore server_socket in this loop
                for (int j = 0; j < MAX_PLAYERS; j++) {
                    if (players[j].socket == i) {
                       // handle_player_guess(players[j].id-1); // Handle player guess first
                        handle_player_guess(j);
                        break;
                    }
                }
            }
        }
    //sleep(1);
// SECOND LOOP: Now handle new connections
        if (FD_ISSET(server_socket, &read_fds) && num_players < max_players) {
            handle_new_connection();
        }
    }
}
