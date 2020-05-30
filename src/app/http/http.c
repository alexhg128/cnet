#include "http.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../native/networking.h"

// Function declaration
char *method_not_allowed();
char *not_found();
char *bad_request();
char *ok();
char *ok_with_payload(char *x);
char *ok_with_payload_and_mime(long len, char *mime);

// Method declaration
char *const get = "GET";

void red()
{
    printf("\033[1;31m");
};

void green()
{
    printf("\033[1;32m");
};

void reset()
{
    printf("\033[0m");
};

// Callback for native networking module
void receive(int socket, char *content)
{
    // Log request to file
    FILE *pFile2;
    pFile2 = fopen("log.txt", "a");
    fprintf(pFile2, "\n%s\n", content);
    fprintf(pFile2, "\n==============================================================================\n");
    fclose(pFile2);

    // Print request symbol
    printf("-> ");

    // Get the first word
    char *part = strtok(content, " ");
    // Verify that the first word is not empty and indicates a GET method
    if (part == NULL || strcmp(part, get) != 0)
    {
        // Send method not allowed response
        char *res = method_not_allowed();
        send_answer(socket, res);
        free(res);

        red();
        printf("[ Method Not Allowed ]\n");
        reset();

        return;
    }
    printf("%.*s ", 10, part);
    // Get the second word (path)
    part = strtok(NULL, " ");
    // If not specified return Bad Request response
    if (part == NULL)
    {
        red();
        printf("[ Bad Request ]\n");
        reset();

        send_answer(socket, bad_request());
        return;
    }

    // Allocate memory for path building
    char *x = (char *)malloc(sizeof(char) * 2048);
    // Initialize in public directory
    strcpy(x, "./public");
    // Add the request path
    strcat(x, part);
    // If the last character its a /, then add index.html
    int len = strlen(x);
    if (x[len - 1] == '/')
    {
        strcat(x, "index.html");
    }

    printf("%.*s ", 30, x);

    // Get extension (characters after .)
    char *path = malloc(strlen(x) * sizeof(char));
    strcpy(path, x);
    char *extension;
    char *p = strtok(path, ".");
    while (p != NULL)
    {
        if (extension != NULL)
        {
            free(extension);
        }
        extension = malloc(strlen(p) * sizeof(char));
        strcpy(extension, p);
        p = strtok(NULL, ".");
    }
    free(path);
    if (extension == NULL)
    {
        strcat(x, ".html");
    }

    // Declare file variable
    FILE *fp;
    char *buffer = 0;
    long length;
    int binary = 0;

    // Open file in read mode and binary for certain extensions
    if (strcmp(extension, "jpg") == 0 || strcmp(extension, "JPG") == 0)
    {
        fp = fopen(x, "rb");
        binary = 1;
    }
    else if (strcmp(extension, "pdf") == 0 || strcmp(extension, "PDF") == 0)
    {
        fp = fopen(x, "rb");
        binary = 1;
    }
    else
    {
        fp = fopen(x, "r");
    }

    // If file not exists return a Not Found response
    if (fp == NULL)
    {
        // Get response
        red();
        printf("[ Not Found ]\n");
        reset();
        char *res = not_found();
        // Send response
        send_answer(socket, res);
        // Free the space ocuppied by response
        free(res);
    }
    else
    {
        // Look for EOF
        fseek(fp, 0, SEEK_END);
        // Get the file lenght
        length = ftell(fp);
        // Set file pointer to the start of the file
        fseek(fp, 0, SEEK_SET);
        // Allocate a buffer og the size of the file
        buffer = malloc(length + 1);
        // Set the last character to string terminator
        buffer[length] = '\0';
        if (buffer)
        {
            // Read from file to buffer
            fread(buffer, 1, length, fp);
        }
        // Close the file
        fclose(fp);

        // Build the response with the payload
        green();
        printf("[ OK ]\n");
        reset();
        char *res = malloc(1000 * sizeof(char));

        // Respond with different mime types depending on extension
        if (extension == NULL)
        {
            res = ok_with_payload(buffer);
        }
        else if (strcmp(extension, "html") == 0 || strcmp(extension, "HTML") == 0)
        {
            res = ok_with_payload(buffer);
        }
        else if (strcmp(extension, "pdf") == 0 || strcmp(extension, "PDF") == 0)
        {
            res = ok_with_payload_and_mime(length, "application/pdf");
        }
        else if (strcmp(extension, "jpg") == 0 || strcmp(extension, "jpg") == 0)
        {
            res = ok_with_payload_and_mime(length, "image/jpeg");
        }
        // Send response
        send_answer(socket, res);

        // Send binary content
        if (binary == 1)
        {
            send_answer_with_len(socket, buffer, length);
        }
        // Free response
        free(res);
        // Free the buffer resources
        free(buffer);
    }
    // Free path building string
    free(x);
}

char *not_available()
{
    // Get current time
    time_t mytime;
    mytime = time(NULL);
    // Allocate a response string
    char *res;
    res = (char *)malloc(1000 * sizeof(char));
    // Build the response
    sprintf(res, "HTTP/1.1 503 Service Unavailable\r\nServer: CNETv1.0\r\nConnection: close\r\nContent-Type: text/html; charset=UTF-8\r\nDate: %.24s GMT\r\nContent-Length: 0\r\n", ctime(&mytime));
    return res;
}

char *not_found()
{
    // Get current time
    time_t mytime;
    mytime = time(NULL);
    // Allocate a response string
    char *res;
    res = (char *)malloc(1000 * sizeof(char));
    // Build the response
    sprintf(res, "HTTP/1.1 404 Not Found\r\nServer: CNETv1.0\r\nConnection: close\r\nDate: %.24s GMT\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 42\r\n\r\n<html><body><p>Not Found</p></body></html>", ctime(&mytime));
    return res;
}

char *method_not_allowed()
{
    // Get current time
    time_t mytime;
    mytime = time(NULL);
    // Allocate a response string
    char *res;
    res = (char *)malloc(1000 * sizeof(char));
    // Build the response
    sprintf(res, "HTTP/1.1 405 Method Not Allowed\r\nServer: CNETv1.0\r\nConnection: close\r\nAllow: GET\r\nContent-Type: text/html; charset=UTF-8\r\nDate: %.24s GMT\r\nContent-Length: 0\r\n", ctime(&mytime));
    return res;
}

char *ok()
{
    // Get current time
    time_t mytime;
    mytime = time(NULL);
    // Allocate a response string
    char *res;
    res = (char *)malloc(1000 * sizeof(char));
    // Build the response
    sprintf(res, "HTTP/1.1 200 OK\r\nServer: CNETv1.0\r\nConnection: close\r\nAllow: GET\r\nContent-Type: text/html; charset=UTF-8\r\nDate: %.24s GMT\r\nContent-Length: 37\r\n\r\n<html><body><p>Hola</p></body></html>", ctime(&mytime));
    return res;
}

char *ok_with_payload(char *x)
{
    // Get the current time
    time_t mytime;
    mytime = time(NULL);
    // Allocate the response string based on the payload size
    char *res;
    res = (char *)malloc(1000 * sizeof(char) + (strlen(x) * sizeof(char)));
    // Build response
    sprintf(res, "HTTP/1.1 200 OK\r\nServer: CNETv1.0\r\nConnection: close\r\nAllow: GET\r\nContent-Type: text/html; charset=UTF-8\r\nDate: %.24s GMT\r\nContent-Length: %d\r\n\r\n%s", ctime(&mytime), strlen(x), x);
    return res;
}

char *ok_with_payload_and_mime(long len, char *mime)
{
    // Get the current time
    time_t mytime;
    mytime = time(NULL);
    // Allocate the response string based on the payload size
    char *res;
    res = (char *)malloc(1000 * sizeof(char));
    // Build response
    sprintf(res, "HTTP/1.1 200 OK\r\nServer: CNETv1.0\r\nConnection: close\r\nAllow: GET\r\nContent-Type: %s\r\nDate: %.24s GMT\r\nContent-Length: %d\r\n\r\n", mime, ctime(&mytime), len);
    return res;
}

char *bad_request()
{
    return "HTTP/1.1 400 Bad Request\r\nServer: CNETv1.0\r\n";
}