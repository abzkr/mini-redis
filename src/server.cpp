#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unordered_map>
#include "parser.h"
#include "sockets.h"
#include "server.h"

#define MAX_CONNECTIONS 10

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
    if(epfd == -1){
        std::cout << "Error creating epoll instance " << '\n';
    }

    ev.events = EPOLLIN;
    ev.data.fd = listener;

    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev) == -1){
        std::cout << "error adding listener fd" << '\n';
    }

    // Event loop
    for(;;) {
        // nfds holds the number of events that happend and returned to the events list
        nfds = epoll_wait(epfd, events, MAX_CONNECTIONS, -1);
        if (nfds == -1) {
            std::cout << "ERROR occured: epoll_await() " << '\n';
        }

        for (int i = 0; i < nfds; ++i) {
            // If the readable (EPOLLIN) socket is a listening socket
            if (events[i].data.fd == listener) {
                connection = new_connection(listener);
        
                if(connection == -1) {
                    std::cout << "error creating new connection: accept()" << '\n';
                    continue; // to the next event
                }
                
                if(set_nonblock(connection) == -1) {
                    close(connection);
                    continue;
                };
                
                ev.events = EPOLLIN;
                ev.data.fd = connection;
                
                if(epoll_ctl(epfd, EPOLL_CTL_ADD, connection, &ev) == -1) {
                    std::cout << "error adding connection: epoll_ctl()" << '\n';
            }
            }
            // In the case that it's a connection socket
            else {
                ssize_t recv_bytes = recv(events[i].data.fd , buff, lenbuff-1, 0);
                
                // Handling client disconnect
                if(recv_bytes == 0){
                    if(epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, nullptr) == -1){
                        std::cout << "error removing connection: epoll_ctl()" << '\n';
                    }
                    else{
                        close(events[i].data.fd);
                        std::cout << "Client has disconnected" << '\n';
                    } 
                }
                // If we recieve data
                else if (recv_bytes > 0){
                    buff[recv_bytes] = '\0';  // Null terminates the string so we only write to the amount of bytes we recieve
                    // Response
                    std::string response = parser(buff, kv);
                    ssize_t send_bytes = send(events[i].data.fd, response.c_str(), response.size(), 0);

                    if(send_bytes == -1 ){
                        std::cout << "error sending data: send()" << '\n';
                    }
                }
                else{
                    std::cout << "error receiving data: recv()" << '\n'; 
                }
                    
            }
        }
    }

        return 0;

}
     
   