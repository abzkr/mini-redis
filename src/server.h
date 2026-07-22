#ifndef SERVER_H
#define SERVER_H

void* get_in_addr (struct sockaddr* ap);
int set_nonblock (int fd);
int start_listener();
int new_connection(int listener);
int server();




#endif