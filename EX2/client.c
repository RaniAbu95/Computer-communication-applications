//
// Created by Rani Abu Raia on 28/12/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>

#define BUFFER_SIZE 4096

void usage() {
    fprintf(stderr, "Usage: client [-r n <pr1=value1 pr2=value2 ...>] <URL>\n");
    exit(EXIT_FAILURE);
}

int check_params(const char *params) {
    // If no parameters, just return
    if (params == NULL || params[0] == '\0') {
        return -1;
    }

    // Skip the initial '?'
    if (params[0] == '?') {
        params++;
    }

    // Split the parameters by '&'
    char *param_copy = strdup(params);  // Make a copy of params to avoid modifying the original
    char *param = strtok(param_copy, "&");  // Tokenize by '&'

    while (param != NULL) {
        // Check if the parameter has an '=' sign
        if (strchr(param, '=') == NULL) {
            free(param_copy);  // Free the allocated memory
            return -1;
        }
        // Move to the next parameter
        param = strtok(NULL, "&");
    }

    // If all parameters are in the correct format
    free(param_copy);
    return 0;
}


void parse_url(const char *url, char *host, char *path, int *port) {
    const char *http_prefix = "http://";
    if (strncmp(url, http_prefix, strlen(http_prefix)) != 0) {
        usage();
    }

    url += strlen(http_prefix);

    const char *colon = strchr(url, ':');
    const char *slash = strchr(url, '/');

    if (colon && (!slash || colon < slash)) {
        strncpy(host, url, colon - url);
        host[colon - url] = '\0';
        *port = atoi(colon + 1);
        if (*port <= 0 || *port >= 65536) {
            usage();
        }
    } else {
        if (slash) {
            strncpy(host, url, slash - url);
            host[slash - url] = '\0';
        } else {
            strcpy(host, url);
        }
        *port = 80; // default HTTP port
    }

    if (slash) {
        strcpy(path, slash);
    } else {
        strcpy(path, "/");
    }
}

bool starts_with_www(const char *host) {
    return strncmp(host, "www.", 4) == 0;
}

// Function to ensure the host starts with "www."
char *ensure_www_prefix(const char *host) {
    // Check if the host already starts with "www."
    if (starts_with_www(host)) {
        return strdup(host); // Return a copy of the original string
    }

    // Allocate memory for the new host with "www." prefix
    size_t new_length = strlen(host) + 5; // 4 for "www." and 1 for '\0'
    char *new_host = malloc(new_length);

    if (new_host == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }

    // Add "www." prefix
    snprintf(new_host, new_length, "www.%s", host);

    return new_host; // Return the modified host
}

void build_request(char *request, const char *host, const char *path, const char *params) {
    char * new_host = ensure_www_prefix(host);
    if (params && strlen(params) > 1) {
        snprintf(request, 1024, "GET %s%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, params, new_host);
    } else {
        snprintf(request, 1024, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, new_host);
    }
    free(new_host);
}

int connect_to_host(const char *host, int port) {

        // Connect to server
        struct sockaddr_in server_addr;
        struct hostent *server;

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }

        server = gethostbyname(host);
        if (server == NULL) {
            fprintf(stderr, "No such host\n");
            exit(EXIT_FAILURE);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
        server_addr.sin_port = htons(port);

        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            exit(EXIT_FAILURE);
        }
        return sock;
}


void send_http_request(int sock, const char *host, const char *path,const char *params) {
    char request[1024];

    // Create the HTTP GET request
    build_request(request,host,path,params);
    printf("HTTP request =\n%s\nLEN = %d\n", request, strlen(request));

    // Send the HTTP request
    if (send(sock, request, strlen(request), 0) == -1) {
        perror("send failed");
        close(sock);
        return;
    }

}


void handle_response(int sock) {
    char buffer[BUFFER_SIZE];
    int bytes_received;
    char *location_header = NULL;
    int status_code = 0;

    // Read the HTTP response
    while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the buffer

        // Print the response to the console
        //printf("%s", buffer);

        // Parse the HTTP status code (only checks the first line)
        if (status_code == 0) { // Parse status code only once
            sscanf(buffer, "HTTP/1.1 %d", &status_code);
        }
        if (status_code >= 200 && status_code < 300) { // Parse status code only once
            printf("\nTotal received response bytes: %d\n",bytes_received);
            break;
        }
        // Look for "Location" header if status is 3XX
        if (status_code >= 300 && status_code < 400) {
            printf("\nTotal received response bytes: %d\n",bytes_received);
            char *location = strstr(buffer, "Location: ");
            if (location) {
                location += 10; // Move past "Location: "
                char *end = strchr(location, '\r'); // Find end of the line
                if (end) *end = '\0'; // Null-terminate the URL

                location_header = strdup(location); // Save the new URL
                break; // Exit loop once the location is found
            }
        }

    }

    // Handle redirection if 3XX and Location header found
    if (status_code >= 300 && status_code < 400 && location_header) {
        if (strncmp(location_header, "http://", 7) == 0) { // Handle only HTTP URLs
            //printf("\nRedirecting to: %s\n", location_header);
            close(sock); // Close the current connection
            // Parse the new URL
            char host[256], path[256];
            int port;
            parse_url(location_header, host, path, &port);

            // Now, use the parsed components to initiate a new connection
            int new_sock = connect_to_host(host, port); // You'll need a function to handle socket connection
            if (new_sock != -1) {
                // Send a new HTTP request to the new URL (using the parsed host and path)
                //add NULL only
                send_http_request(new_sock, host, path,NULL); // Implement this function to send the request
                handle_response(new_sock); // Recursively handle the new response
                close(sock);
            } else {
                printf("Unsupported redirection to non-HTTP URL.\n");
            }
            free(location_header); // Free dynamically allocated memory
        }
        else{
            usage();
        }
    }

}


int main(int argc, char *argv[]) {
    char **argv_copy = malloc(argc * sizeof(char *));
    if (argc < 2) {
        usage();
    }
    char host[256], path[1024] = "", params[1024] = "";
    int port;

    int index;
    if (strcmp(argv[1], "-r") == 0) {
        if(argc < 4)
        {
            usage();
        }
        int value = atoi(argv[2]);
        if(value ==0)
        {
            usage();
        }
        if(atoi(argv[2])!=argc-4){
            usage();
        }
        for(int i =2 ; i < argc-2; i++)
        {
            index = i+1;
            if(i==2)
            {
                strcat(params,"?");
            }
            if(index==argc - 2)
            {
                strcat(params, argv[index]);
            }
            else{
                strcat(params, argv[index]);
                strcat(params,"&");
            }

        }
        if(check_params(params) !=0)
        {
            usage();
        }
    }

    parse_url(argv[argc-1], host, path, &port);

    int sock = connect_to_host(host,port);

    send_http_request(sock,host,path,params);


    // Handle the server response
    handle_response(sock);

    return 0;
}
