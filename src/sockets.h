#ifndef SOCKETS_H
#define SOCKETS_H

void* get_in_addr (struct sockaddr* ap);
int set_nonblock (int fd);
int start_listener();
int new_connection(int listener);


#endif