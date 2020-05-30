/**
 * Linux native networking module
 */

#include "../networking.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// The port to listen
#define PORT 8080
// The max number of connections
#define MAX_CONNECTIONS 1000
// The current number of connections
int connections = 0;

// Threading structure for argument passing
typedef struct thread_arg
{
    // Incoming socket
    int socket;
    // Message
    char* buffer;
    // Callback function
    void (*callback)(int, char*);
} thread_arg;

// Threading function wich process the request in a separate thread
void process(void *input) {
    // Call the callback function
    (((thread_arg*) input)->callback)(((thread_arg*) input)->socket, ((thread_arg*) input)->buffer);
    // Close the socket connection
    shutdown(((thread_arg*) input)->socket, 2);
    // Free the buffer allocated memory and the argument memory
    free(((thread_arg*) input)->buffer);
    free(input);
    // sleep(5);
    // Substract from current connections
    connections--;
}

// Function that starts listening for connections
void start_socket(void (*callback)(int, char*), char* not_available()) {
    // Server variables
    int server_fd;
    int new_socket;
    int valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Initialize the server
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize the socket
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Failed at setsocketopt");
        exit(EXIT_FAILURE);
    }

    // Declare the connection details
    address.sin_family = AF_INET;
    // Set address to 0.0.0.0 so we can listen connections from other computers
    address.sin_addr.s_addr = inet_addr("0.0.0.0");
    //address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the server to the address
    if(bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Failed binding the port");
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    listen(server_fd, 10);

    // Listening loop
    while(1) {
        // Accept an incoming connection
        if((new_socket = accept(server_fd, (struct sockaddr *)&address,  (socklen_t*)&addrlen)) < 0) {
            perror("Failed accepting request");
            // exit(EXIT_FAILURE);
        }
        // Alocate a incoming buffer of 2048 characters
        char* buffer;
        buffer = (char*) malloc(2048 * sizeof(char));
        // Read message and copy it to the buffer
        valread = read(new_socket, buffer, 2048);
        // Verify if the server has capacity to process the request
        if(connections >= MAX_CONNECTIONS) {
            char* res = not_available();
            send(new_socket, res, strlen(res), 0);
            shutdown(new_socket, 2);
            free(buffer);
            continue;
        }
        // Add to the count of connections
        connections++;

        // Start a new thread and pass it the buffer, the socket and the callback
        thread_arg* args = (thread_arg*) malloc(sizeof(thread_arg));
        args->socket = new_socket;
        args->buffer = buffer;
        args->callback = callback;
        pthread_t t;
        pthread_create(&t, NULL, process, args);
    }
}



void send_answer(int socket, char* response) {
    // Send a message to a specified socket
    send(socket, response, strlen(response), 0);
}

void send_answer_with_len(int socket, char* response, long lenght) {
    // Send a message to a specified socket
    send(socket, response, lenght, 0);
}

// Test method
void send_request() {

    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char *hello = "Hello from client"; 
    char buffer[1024] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        exit(EXIT_FAILURE);
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        exit(EXIT_FAILURE);
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        exit(EXIT_FAILURE); 
    } 
    send(sock , hello , strlen(hello) , 0 ); 
    printf("Hello message sent\n"); 
    valread = read( sock , buffer, 1024); 
    printf("%s\n",buffer );
}