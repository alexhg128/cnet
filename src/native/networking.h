/**
 * 
 * Header file for native networking module
 * 
 */

#ifndef __NETWORKING__
#define __NETWORKING__

void start_socket();
void send_request();
void send_answer(int socket, char* response);
void send_answer_with_len(int socket, char* response, long lenght);

#endif