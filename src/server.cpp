#include <iostream>
#include "sockets.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unordered_map>
#include "parser.h"
#include <vector>
// #include "server.h"

#define MAX_CONNECTIONS 10

int server()
{

    /* Main server function with epoll loop*/

    std::unordered_map<std::string, std::string> kv;
    std::string connection_addresses[MAX_CONNECTIONS];

    char buff[4096];
    ssize_t lenbuff = sizeof buff;

    struct epoll_event ev, events[MAX_CONNECTIONS];
    int nfds;
    int epfd;
    int connection;

    Socket listener{AF_UNSPEC, SOCK_STREAM, AI_PASSIVE};
    int listener_fd{listener.getFd()};
    listener.set_nonblock();

    epfd = epoll_create1(0);
    if (epfd == -1)
    {
        std::cout << "Error creating epoll instance " << '\n';
    }

    ev.events = EPOLLIN;
    ev.data.fd = listener_fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listener_fd, &ev) == -1)
    {
        std::cout << "error adding listener fd" << '\n';
    }

    std::vector<Socket> connections{};
    connections.reserve(MAX_CONNECTIONS); // to prevent reallocations, we reserve for the max objects we want to hold anyway

    // Event loop
    for (;;)
    {
        // nfds holds the number of events that happend and adds them to the events list
        nfds = epoll_wait(epfd, events, MAX_CONNECTIONS, -1);
        if (nfds == -1)
        {
            std::cout << "ERROR occured: epoll_await() " << '\n';
        }

        for (int i = 0; i < nfds; ++i)
        {
            // If the readable (EPOLLIN) socket is a listening socket
            if (events[i].data.fd == listener_fd)
            {
                // Find a way to initalise differently, this keeps destroying the fd by closing it
                Socket connection{listener_fd};

                ev.events = EPOLLIN;
                ev.data.fd = connection.getFd();

                if (connection.getFd() == -1)
                {
                    std::cout << "error creating new connection: accept()" << '\n';
                    continue; // to the next event
                }

                if ((connection.set_nonblock()) == -1)
                {
                    close(connection.getFd());
                    continue;
                };

                if (epoll_ctl(epfd, EPOLL_CTL_ADD, connection.getFd(), &ev) == -1)
                {
                    std::cout << "error adding connection: epoll_ctl()" << '\n';
                }

                connections.push_back(std::move(connection));
            }
            // In the case that we recieve an event (message) from a connection socket
            else
            {
                ssize_t recv_bytes = recv(events[i].data.fd, buff, lenbuff - 1, 0);
                // Handling client disconnect

                if (recv_bytes == 0)
                {
                    if (epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, nullptr) == -1)
                    {
                        std::cout << "error removing connection: epoll_ctl()" << '\n';
                    }
                    else
                    {
                        close(events[i].data.fd);
                        std::cout << "Client has disconnected" << '\n';
                    }
                }

                // If we recieve data
                else if (recv_bytes > 0)
                {
                    buff[recv_bytes] = '\0'; // Null terminates the string so we only write to the amount of bytes we recieve
                    // Response
                    std::string response = parser(buff, kv);
                    ssize_t send_bytes = send(events[i].data.fd, response.c_str(), response.size(), 0);

                    if (send_bytes == -1)
                    {
                        std::cout << "error sending data: send()" << '\n';
                    }
                }
                else
                {
                    std::cout << "error receiving data: recv()" << '\n';
                }
            }
        }
    }

    return 0;
}
