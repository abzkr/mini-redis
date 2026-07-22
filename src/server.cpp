#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unordered_map>
#include "parser.h"
#include <sstream>

#define PORT "6379"
#define BACKLOG 10
#define MAX_CONNECTIONS 10

void* get_in_addr (struct sockaddr* ap) {
    if(ap->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) ap) ->sin_addr);
    }
    else{
        return &(((struct sockaddr_in6*) ap) ->sin6_addr);
    }
}   

int set_nonblock (int fd){
    fcntl(fd, F_SETFL, O_NONBLOCK);

    return fd;
}


int start_listener(){

    int exit_status;
    struct addrinfo hints;

    struct addrinfo *res;
    struct addrinfo *ptr;
    
    // Ensure that its empty - fill with 0s
    memset(&hints, 0, sizeof(hints));

    // How we want our socket configurations 
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;



    if((exit_status = getaddrinfo(NULL, PORT, &hints , &res)) != EXIT_SUCCESS ){
        std::cerr << "getaddrinfo() error " << gai_strerror(exit_status);
        return 1;
    }
    
    // Loop to find a valid entry in the linked list and error checking each system call

    int listener_fd = -1;
    for(ptr = res; ptr != nullptr; ptr = ptr->ai_next){
        if((listener_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) >= 0){
            
            // so we can reuse the socket
            int yes = 1;
            setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

            if(bind(listener_fd, ptr->ai_addr, ptr->ai_addrlen) >= 0){
                if (listen(listener_fd, BACKLOG) >= 0){
                    std::cout << "Succesfully opened socket and listening..." << '\n';
                    break;
                }
                else {
                    std::cout << "Failed to listen on socket... "  << '\n';
                }
            }
            else {
                std::cout << "Failed to bind to local " << '\n';
            }
        }
        else {
            std::cout << "Failed to create socket " <<'\n';
            continue;
        }
    }
    
    // Free the linkedlist
    freeaddrinfo(res);

    return listener_fd;

}

int new_connection(int listener) {
    
    struct sockaddr_storage client_addr;
    struct sockaddr* clientaddrptr = (struct sockaddr* )&client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    // client address -> inet_ntop changes this from network ip to presentable so we can actually read it 
    char client_addr_s[INET6_ADDRSTRLEN];
       
    // Creating sockets for each new connection
    int connecfd = accept(listener, clientaddrptr, &client_addr_size);

    inet_ntop(client_addr.ss_family, get_in_addr(clientaddrptr), client_addr_s, sizeof client_addr_s);
    std::cout << "Incoming connection from: " << client_addr_s << '\n';


    return connecfd;
}

int server(){

    /* Main server function with epoll loop*/


    std::unordered_map <std::string, std::string> kv;
    std::string connection_addresses[MAX_CONNECTIONS];

    char buff [4096];
    ssize_t lenbuff = sizeof buff;

    struct epoll_event ev, events[MAX_CONNECTIONS];
    int nfds;
    int epfd;
    int connection;

    int listener = start_listener();
    set_nonblock(listener);

    epfd = epoll_create1(0);
    // Some error handling here
    
    ev.events = EPOLLIN;
    ev.data.fd = listener;

    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev) == -1){
        std::cout << "error adding listener fd" << '\n';
    }

    for(;;) {
        nfds = epoll_wait(epfd, events, MAX_CONNECTIONS, -1);
        if (nfds == -1) {
            std::cout << "ERROR occured: epoll_await() " << '\n';
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == listener) {
                connection = new_connection(listener);
                
                // error handling here -> if connection returns -1

                set_nonblock(connection);
                ev.events = EPOLLIN;
                ev.data.fd = connection;
                
                if(epoll_ctl(epfd, EPOLL_CTL_ADD, connection, &ev) == -1){
                    std::cout << "error adding connection fd" << '\n';
            }

            }
            else {
                ssize_t recv_bytes = recv(events[i].data.fd , buff, lenbuff-1, 0);
                std::string response = parser(buff, kv);
                if (recv_bytes > 0){
                    buff[recv_bytes] = '\0';
                }
                ssize_t send_bytes = send(events[i].data.fd, response.c_str(), response.size(), 0);
            }
        }

    }
    
        return 0;

}