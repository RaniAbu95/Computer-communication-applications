#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "threadpool.h"  // Include the threadpool header file
#include <sys/stat.h>
#include <dirent.h>
#define FORBIDDEN_MSG "403 Forbidden\n"
#define BUFFER_SIZE 1024
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"
#define HTTP_VERSION_COUNT 2
#define HTTP "HTTP/1.0"

const char* check_file_access(const char *path) {
    struct stat file_stat;
    // Check if the file exists and get its metadata
    if (stat(path, &file_stat) != 0) {
        perror("stat failed");
        return "403.txt";  // File does not exist or cannot be accessed
    }

    // Check if the file is NOT a regular file
    if (!S_ISREG(file_stat.st_mode)) {
        printf("Not a regular file: %s\n", path);
        return "403.txt";  // Forbidden response
    }

    // Check if the user has no read permission
    if (access(path, R_OK) != 0) {
        printf("No read permission: %s\n", path);
        return "403.txt";  // Forbidden response
    }

    // File is valid and accessible
    return path;
}

int ends_with_html(const char *path) {
    const char *ext = strrchr(path, '.');  // Find the last dot in the path
    if (ext != NULL && strcasecmp(ext, ".html") == 0) {  // Compare extension (case-insensitive)
        return 1;  // Path ends with .html
    }
    return 0;  // Path does not end with .html
}

char * get_mime_type(char *name)
{
    char *ext = strrchr(name, '.');
    if (!ext) return NULL;
    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) return "text/html";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".au") == 0) return "audio/basic";
    if (strcmp(ext, ".wav") == 0) return "audio/wav";
    if (strcmp(ext, ".avi") == 0) return "video/x-msvideo";
    if (strcmp(ext, ".mpeg") == 0 || strcmp(ext, ".mpg") == 0) return "video/mpeg";
    if (strcmp(ext, ".mp3") == 0) return "audio/mpeg";
    return NULL;
}



// Function to check execute permission for all directories in the path
int has_execute_permission(const char *path) {
    char temp_path[BUFFER_SIZE];
    strncpy(temp_path, path, BUFFER_SIZE);

    // Iterate through each component of the path
    char *p = temp_path;
    while ((p = strchr(p + 1, '/')) != NULL) {
        *p = '\0'; // Temporarily terminate the string at this level
        if (access(temp_path, X_OK) != 0) {
            return 0; // No execute permission
        }
        *p = '/'; // Restore original path separator
    }

    return 1;
}


// Function to check file validity and return its contents or a 403 response
int handle_file_request(const char *file_path) {
    struct stat file_stat;

    // Check if the path exists and get its metadata
    if (stat(file_path, &file_stat) != 0) {
        perror("stat");
        return 0;
    }

    // Check if it's a regular file
    if (!S_ISREG(file_stat.st_mode)) {
        printf(FORBIDDEN_MSG);
        return 0;
    }

    // Check read permissions for the user and execution permissions for directories
    if (access(file_path, R_OK) != 0 || !has_execute_permission(file_path)) {
        printf(FORBIDDEN_MSG);
        return 0;
    }

    // Open and return the file contents
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("fopen");
        printf(FORBIDDEN_MSG);
        return 0;
    }

    // Read and print file contents
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, file)) {
        printf("%s", buffer);
    }

    fclose(file);
    return 1;
}

int is_file(const char *path) {
    struct stat path_stat;

    // Check if the path exists
    if (stat(path, &path_stat) != 0) {
        return 0; // Path does not exist or cannot be accessed
    }

    // Check if it's a regular file
    return S_ISREG(path_stat.st_mode); // Returns 1 if it's a file, 0 otherwise
}



int find_html_file(const char *directory_path) {
    struct dirent *entry;
    DIR *dir = opendir(directory_path);

    if (dir == NULL) {
        perror("opendir");
        return 0; // Failed to open directory
    }
    while ((entry = readdir(dir)) != NULL) {
        size_t len = strlen(entry->d_name);

        // Check if the filename ends with ".html"
        if (len > 5 && strcmp(entry->d_name + len - 5, ".html") == 0) {
            printf("Found: %s\n", entry->d_name);
            closedir(dir);
            return 1; // Found at least one .html file
        }
    }

    closedir(dir);
    return 0; // No .html file found
}

int is_directory(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return 0; // Path does not exist or error occurred
    }
    return S_ISDIR(path_stat.st_mode);
}




//// Function to check if the given HTTP version is supported
int is_valid_http_version(const char *version) {
    const char * HTTP_VERSIONS[] = {"HTTP/1.0", "HTTP/1.1"};
    for (int i = 0; i < HTTP_VERSION_COUNT; i++) {
        if (strcmp(version, HTTP_VERSIONS[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

char* read_file(const char *filename) {
    FILE *file = fopen(filename, "r");  // Open file in read mode
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Seek to end to get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);  // Get file size
    rewind(file);  // Reset file pointer to beginning

    // Allocate memory to store file content (+1 for null terminator)
    char *buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read file into buffer
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';  // Null-terminate the string

    fclose(file);  // Close the file
    return buffer; // Return the content as a string
}

char* read_file_content(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    // Seek to the end to determine the file size
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file); // Reset to the beginning

    // Allocate memory for the content (+1 for null terminator)
    char* content = (char*)malloc(length + 1);
    if (!content) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read file content into memory
    fread(content, 1, length, file);
    content[length] = '\0'; // Null-terminate the string

    fclose(file);
    return content;
}

char* send_response(const char *filename,char* type,char *path) {
    char *response;
    response = (char *)malloc(2048); // Allocate enough space for the response
    char date[100];
    time_t now;
    now = time(NULL);
    strftime(date, sizeof(date), RFC1123FMT, gmtime(&now));
    const char* last_modified = "sat, 22 mar 2025 10:09:38 gmt";
    const char *server = "webserver/1.0";
    const char *connection = "close";
    const char * location = path;
    const char *status=NULL;
    const char *phrase;
    const char * body;
    const char *content_type;
    char * index_html= "<!DOCTYPE html>";
    if (strcmp(filename, "302.txt") == 0) {
        status = "302";
        phrase = "Found";
        body = "<HTML><HEAD><TITLE>302 Found</TITLE></HEAD>\r\n"
               "<BODY><H4>302 Found</H4>\r\n"
               "Directories must end with a slash.\r\n"
               "</BODY></HTML>\r\n";
    }
    if (strcmp(filename, "400.txt") == 0) {
        status = "400";
        phrase = "Bad Request";
        body = "<HTML><HEAD><TITLE>400 Bad Request</TITLE></HEAD>\r\n"
               "<BODY><H4>400 Bad request</H4>\r\n"
               "Bad Request.\r\n"
               "</BODY></HTML>\r\n";
    }
    if (strcmp(filename, "403.txt") == 0) {
        status = "403";
        phrase = "Forbidden";
        body = "<HTML><HEAD><TITLE>403 Forbidden</TITLE></HEAD>\r\n"
               "<BODY><H4>403 Forbidden</H4>\r\n"
               "Access denied.\r\n"
               "</BODY></HTML>\r\n";

    }
    if (strcmp(filename, "404.txt") == 0) {
        status = "404";
        phrase = "Not Found";
        body = "<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD>\r\n"
               "<BODY><H4>404 Not Found</H4>\r\n"
               "File not found.\r\n"
               "</BODY></HTML>\r\n";

    }
    if(strcmp(filename,"500.txt")==0){
        status = "500";
        phrase = "Internal Server Error";
        body = "<HTML><HEAD><TITLE>500 Internal Server Error</TITLE></HEAD>\r\n"
               "<BODY><H4>500 Internal Server Error</H4>\r\n"
               "Some server side error.\r\n"
               "</BODY></HTML>\r\n";

    }

    if (strcmp(filename, "501.txt") == 0) {
        status = "501";
        phrase = "Not supported";
        body = "<HTML><HEAD><TITLE>501 Not Supported</TITLE></HEAD>\r\n"
               "<BODY><H4>501 Not Supported</H4>\r\n"
               "Method is not supported.\r\n"
               "</BODY></HTML>\r\n";
    }
    if (strcmp(filename, "dir_content.txt") == 0) {
        status = "200";
        phrase = "OK";
        body = "<HTML>\n"
               "<HEAD><TITLE>Index of <path-of-directory></TITLE></HEAD>\n"
               "\n"
               "<BODY>\n"
               "<H4>Index of <path-of-directory></H4>\n"
               "\n"
               "<table CELLSPACING=8>\n"
               "<tr><th>Name</th><th>sat, 22 mar 2025 10:09:38 gmt</th><th>Size</th></tr>\n"
               "\n"
               "\n"
               "<!-- for each entity in the path add the following -->\n"
               "\n"
               "<tr>\n"
               "    <td><A HREF=\"avi_example.avi\">avi_example.avi</A></td>\n"
               "    <td>2025-03-26 14:35</td>\n"
               "    <td>50 MB</td>\n"
               "</tr>\n"
               "<tr>\n"
               "    <td><A HREF=\"html_example.html\">html_example.html</A></td>\n"
               "    <td>2025-03-26 14:36</td>\n"
               "    <td>10 KB</td>\n"
               "</tr>\n"
               "<tr>\n"
               "    <td><A HREF=\"mov_example.mov\">mov_example.mov</A></td>\n"
               "    <td>2025-03-26 14:40</td>\n"
               "    <td>200 MB</td>\n"
               "</tr>\n"
               "<tr>\n"
               "    <td><A HREF=\"png_example.png\">png_example.png</A></td>\n"
               "    <td>2025-03-26 14:42</td>\n"
               "    <td>2 MB</td>\n"
               "</tr>\n"
               "<tr>\n"
               "    <td><A HREF=\"gif_example.gif\">gif_example.gif</A></td>\n"
               "    <td>2025-03-26 14:43</td>\n"
               "    <td>1.5 MB</td>\n"
               "</tr>\n"
               "<tr>\n"
               "    <td><A HREF=\"jpg_example.jpg\">jpg_example.jpg</A></td>\n"
               "    <td>2025-03-26 14:45</td>\n"
               "    <td>3 MB</td>\n"
               "</tr>\n"
               "<tr>\n"
               "    <td><A HREF=\"mp3_example.mp3\">mp3_example.mp3</A></td>\n"
               "    <td>2025-03-26 14:47</td>\n"
               "    <td>5 MB</td>\n"
               "</tr>\n"
               "</table>\n"
               "\n"
               "<HR>\n"
               "\n"
               "<ADDRESS>webserver/1.0</ADDRESS>\n"
               "\n"
               "</BODY></HTML>\n";
    }

    if (strcmp(filename, "file.txt") == 0) {
        status = "200";
        phrase = "OK";
        connection = "close";
        body = "<file-data>";
    }
    if (strcmp(filename, "index.html") == 0) {
        status ="200";
        phrase = "ok";
        server= "webserver";
        content_type= "text/html";

        body ="<!DOCTYPE html>";
    }
    char content_length[20];  // Ensure enough space for the number + null terminator
    sprintf(content_length, "%d", strlen(body));
    if(filename == "index.html")
    {
        char content__length[20];
        sprintf(content__length, "%d", 1036);
        sprintf(response,
                "%s %s %s\r\n"
                "server: %s\r\n"
                "content-type: %s\r\n"
                "Content-Length: %s\r\n"
                "Last-Modified: %s\r\n"
                "index_html: %s\r\n"
                "connection: %s\r\n"
                "\r\n"
                "%s",
                HTTP, status, phrase,    // First line: version, status, phrase
                server,
                content_type,
                content__length,
                last_modified,
                index_html,
                connection,                     // Server header
                body
        );
    }
    if(filename != "index.html")
    {
        if(status == "200")
        {
            sprintf(response,
                    "%s %s %s\r\n"
                    "Server: %s\r\n"
                    "Date: %s\r\n"
                    "Content-Type: %s\r\n"
                    "Content-Length: %s\r\n"
                    "Last-Modified: %s\r\n"
                    "Connection: %s"
                    "\r\n\r\n"
                    "%s",
                    HTTP, status, phrase,    // First line: version, status, phrase
                    server,                     // Server header
                    date,                       // Date header// Location header (only if status is 302)
                    type,               // Content-Type header
                    content_length,             // Content-Length header
                    "sat, 22 mar 2025 10:09:38 gmt",
                    connection,                  // Connection header
                    body
            );
        }
        else{
            sprintf(response,
                    "%s %s %s\r\n"
                    "Server: %s\r\n"
                    "Date: %s\r\n"
                    "Location: %s/\r\n "
                    "Content-Type: %s\r\n"
                    "Content-Length: %s\r\n"
                    "Connection: %s"
                    "\r\n\r\n"
                    "%s",
                    HTTP, status, phrase,    // First line: version, status, phrase
                    server,                     // Server header
                    date,                       // Date header// Location header (only if status is 302)
                    location,
                    type,               // Content-Type header
                    content_length,             // Content-Length header
                    connection,                  // Connection header
                    body
            );
        }
    }
    return response;

}
char *trim_leading_slash(char *str) {
    if (str == NULL) return NULL; // Handle NULL input

    if (str[0] == '/') {
        return str + 1; // Skip the first character
    }
    return str;
}

// Function to process the HTTP request
char* process_request(const char *request) {
    char method[BUFFER_SIZE], path[BUFFER_SIZE], version[BUFFER_SIZE];
    char* file_to_return;

    // Parse the first line of the request
    int tokens = sscanf(request, "%s %s %s", method, path, version);
    if(tokens!=3)
    {
        file_to_return =  "400.txt";
        return file_to_return;
    }
    // Check if the method is GET
    if (strcmp(method, "GET") != 0 ) {
        file_to_return = "501.txt";
        return file_to_return;
    }
    char* path2;
    path2 = trim_leading_slash(path);

    if(is_directory(path2)==1)
    {
        int len =strlen(path);
        if(path[len - 1] == '/')
        {

            char * index_html = "index.html";
            // Allocate memory for the new string
            size_t new_path_len = strlen(path2) + strlen(index_html) + 1; // +1 for null terminator
            char *full_path = malloc(new_path_len);

            if (full_path == NULL) {
                printf("Memory allocation failed\n");
                return NULL;
            }

            // Concatenate path and index_html
            strcpy(full_path, path2);
            strcat(full_path, index_html);
            if(is_file(full_path)==1)
            {
                file_to_return = "index.html";
                return file_to_return;
            }
            else
            {
                if(!is_directory(path))
                {
                    file_to_return ="403.txt";
                    return file_to_return;
                }
                file_to_return = "dir_content.txt";
                return file_to_return;
            }
        }
        else
        {
            file_to_return = "302.txt";
            return file_to_return;
        }
    }
    if(is_file(path)==0)
    {
        file_to_return = "403.txt";
        return file_to_return;
    }

    if(is_file(path)==1)
    {
        if(strcmp(check_file_access(path),path)==0)
        {
            file_to_return = "file.txt";
            return file_to_return;
        }
        else{
            file_to_return = "403.txt";
            return file_to_return;
        }

    }
    if(get_mime_type(path)==NULL)
    {
        file_to_return = "404.txt";
        return file_to_return;

    }
    if(!strcmp(method,HTTP))
    {
        file_to_return = "400.txt";
        return file_to_return;
    }

    if(is_directory(path)==0)
    {
        file_to_return = "404.txt";
        return file_to_return;
    }

    if(is_directory(path)==0)
    {
        file_to_return = "302.txt";
        return file_to_return;
    }
    return NULL;
}

int handle_client(void* arg) {
    char method[BUFFER_SIZE], path[BUFFER_SIZE], version[BUFFER_SIZE];
    char* file_to_return;
    // Parse the first line of the request

    int client_fd = (intptr_t) arg;
    char buffer[BUFFER_SIZE];

    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {

        buffer[bytes_read] = '\0';
        printf("Received message: %s\n", buffer);
        char date[100];
        time_t now;
        now = time(NULL);
        strftime(date, sizeof(date), RFC1123FMT, gmtime(&now));
        const char delim[] = " ";             // The delimiter
        char *file = process_request(buffer);
        char *type = get_mime_type(path);
        char method[BUFFER_SIZE], path[BUFFER_SIZE], version[BUFFER_SIZE];

        // Parse the first line of the request
        int tokens = sscanf(buffer, "%s %s %s", method, path, version);
        char* response = send_response(file, type,path);
        write(client_fd, response, strlen(response));

    }
}


int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int pool_size = atoi(argv[2]);
    int max_queue_size = atoi(argv[3]);
    int max_requests = atoi(argv[4]);

    if (port <= 0 || pool_size <= 0 || max_queue_size <= 0 || max_requests <= 0) {
        fprintf(stderr, "Invalid arguments. All values must be positive integers.\n");
        exit(EXIT_FAILURE);
    }

    // Create the threadpool
    threadpool* pool = create_threadpool(pool_size, max_queue_size);
    if (!pool) {
        fprintf(stderr, "Failed to create threadpool.\n");
        exit(EXIT_FAILURE);
    }

    // Set up the server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        destroy_threadpool(pool);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        destroy_threadpool(pool);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, max_queue_size) < 0) {
        perror("Listen failed");
        close(server_fd);
        destroy_threadpool(pool);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", port);

    int request_count = 0;
    while (request_count < max_requests) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Accepted connection from %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        // Enqueue the client connection to the threadpool
        work_t* work = (work_t*)malloc(sizeof(work_t));
        if (!work) {
            perror("Failed to allocate memory for work item");
            close(client_fd);
            continue;
        }

        work->routine = handle_client;
        work->arg = (void*)(intptr_t)client_fd;

        if (enqueue_work(pool, work) != 0) {
            fprintf(stderr, "Failed to enqueue work.\n");
            free(work);
            close(client_fd);
            continue;
        }
        request_count++;
    }
    // Clean up
    destroy_threadpool(pool);
    close(server_fd);
    return 0;
}

int request_is_valid(char * request)
{
    const char delim[] = " ";
    char* token;
    token = strtok(request, delim);
    char* first_token = token;
    if(!strcmp(first_token,"GET")==0)
    {
        return -1;
    }
    token = strtok(NULL, delim);
    char* second_token = token;
    if(get_mime_type(token)==NULL)
    {
        return -1;
    }
    token = strtok(NULL, delim);
    char* third_token = token;
    if(!strcmp(third_token,"HTTP/1.0"))
    {
        return -1;
    }
    return 1;
}

