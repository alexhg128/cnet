#include "http.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../native/networking.h"

// Function declaration
const char* method_not_allowed();
const char* not_found();
char* bad_request();
const char* ok();
const char* ok_with_payload(char* x);

// Method declaration
char* const get = "GET";

// Callback for native networking module
void receive(int socket, char* content) {
    // Get the first word
    char* part = strtok(content, " ");
    // Verify that the first word is not empty and indicates a GET method
    if(part == NULL || strcmp(part, get) != 0) {
        // Send method not allowed response
        char* res = method_not_allowed();
        send_answer(socket, res);
        free(res);
    }
    // Get the second word (path)
    part = strtok(NULL, " ");
    // If not specified return Bad Request response
    if(part == NULL) {
        send_answer(socket, bad_request());
    }
    
    // Allocate memory for path building
    char* x = (char*) malloc(sizeof(char) * 2048);
    // Initialize in public directory
    strcpy(x, "./public");
    // Add the request path
    strcat(x, part);
    // If the last character its a /, then add index.html
    int len = strlen(x);
    if(x[len -1] == '/') {
        strcat(x, "index.html");
    }

    // Declare file variable
    FILE *fp;
    char* buffer = 0;
    long length;

    // Open file in read mode
    fp = fopen(x, "r");

    // If file not exists return a Not Found response
    if(fp == NULL) {
        // Get response
        char* res = not_found();
        // Send response
        send_answer(socket, res);
        // Free the space ocuppied by response
        free(res);
    } else {
        // Look for EOF
        fseek (fp, 0, SEEK_END);
        // Get the file lenght
        length = ftell (fp);
        // Set file pointer to the start of the file
        fseek (fp, 0, SEEK_SET);
        // Allocate a buffer og the size of the file
        buffer = malloc (length + 1);
        // Set the last character to string terminator
        buffer[length] = '\0';
        if (buffer)
        {
            // Read from file to buffer
            fread (buffer, 1, length, fp);
        }
        // Close the file
        fclose (fp);
        
        // Build the response with the payload
        char* res = ok_with_payload(buffer);
        // Send response
        send_answer(socket, res);
        // Free response
        free(res);
        // Free the buffer resources
        free(buffer);
    }
    // Free path building string
    free(x);
}

const char* not_available() {
    // Get current time
    time_t mytime;
	mytime = time(NULL);
    // Allocate a response string
    char* res;
    res = (char*) malloc(1000 * sizeof(char));
    // Build the response
    sprintf(res, "HTTP/1.1 503 Service Unavailable\r\nServer: CNETv1.0\r\nConnection: close\r\nContent-Type: text/html; charset=UTF-8\r\nDate: %.24s GMT\r\nContent-Length: 0\r\n", ctime(&mytime));
    return res;
}

const char* not_found() {
    // Get current time
    time_t mytime;
	mytime = time(NULL);
    // Allocate a response string
    char* res;
    res = (char*) malloc(1000 * sizeof(char));
    // Build the response
    sprintf(res, "HTTP/1.1 404 Not Found\r\nServer: CNETv1.0\r\nConnection: close\r\nDate: %.24s GMT", ctime(&mytime));
    return res;
}

const char* method_not_allowed() {
    // Get current time
    time_t mytime;
	mytime = time(NULL);
    // Allocate a response string
    char* res;
    res = (char*) malloc(1000 * sizeof(char));
    // Build the response
    sprintf(res, "HTTP/1.1 405 Method Not Allowed\r\nServer: CNETv1.0\r\nConnection: close\r\nAllow: GET\r\nContent-Type: text/html; charset=UTF-8\r\nDate: %.24s GMT\r\nContent-Length: 0\r\n", ctime(&mytime));
    return res;
}

const char* ok() {
    // Get current time
    time_t mytime;
	mytime = time(NULL);
    // Allocate a response string
    char* res;
    res = (char*) malloc(1000 * sizeof(char));
    // Build the response
    sprintf(res, "HTTP/1.1 200 OK\r\nServer: CNETv1.0\r\nConnection: close\r\nAllow: GET\r\nContent-Type: text/html; charset=UTF-8\r\nDate: %.24s GMT\r\nContent-Length: 37\r\n\r\n<html><body><p>Hola</p></body></html>", ctime(&mytime));
    return res;
}

const char* ok_with_payload(char* x) {
    // Get the current time
    time_t mytime;
	mytime = time(NULL);
    // Allocate the response string based on the payload size
    char* res;
    res = (char*) malloc(1000 * sizeof(char) + (strlen(x) * sizeof(char)));
    // Build response
    sprintf(res, "HTTP/1.1 200 OK\r\nServer: CNETv1.0\r\nConnection: close\r\nAllow: GET\r\nContent-Type: text/html; charset=UTF-8\r\nDate: %.24s GMT\r\nContent-Length: %d\r\n\r\n%s", ctime(&mytime), strlen(x), x);
    return res;
}

char* bad_request() {
    return  "HTTP/1.1 400 Bad Request\r\nServer: CNETv1.0\r\n";
}