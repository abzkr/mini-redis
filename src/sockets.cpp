#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "sockets.h"

#define PORT "6379"
#define BACKLOG 10

Socket::Socket(Socket &&sock) noexcept
    : m_fd(sock.m_fd)
{
    sock.m_fd = -1;
}

void *get_in_addr(struct sockaddr *ap)
{
    if (ap->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)ap)->sin_addr);
    }
    else
    {
        return &(((struct sockaddr_in6 *)ap)->sin6_addr);
    }
}

int Socket::start_listener(int family, int socktype, int flags)
{
    int exit_status{};
    struct addrinfo hints{};

    hints.ai_family = family;
    hints.ai_socktype = socktype;
    hints.ai_flags = flags;

    struct addrinfo *res{nullptr};
    struct addrinfo *ptr{nullptr};

    if ((exit_status = getaddrinfo(NULL, PORT, &hints, &res)) != EXIT_SUCCESS)
    {
        std::cerr << "getaddrinfo() failed: " << gai_strerror(exit_status);
        return -1;
    }

    int listener_fd = -1;
    for (ptr = res; ptr != nullptr; ptr = ptr->ai_next)
    {
        if ((listener_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) >= 0)
        {
            // so we can reuse the socket
            int yes = 1;
            setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

            if (bind(listener_fd, ptr->ai_addr, ptr->ai_addrlen) >= 0)
            {
                if (listen(listener_fd, BACKLOG) >= 0)
                {
                    std::cerr << "Succesfully opened socket and listening..." << '\n';
                    break;
                }
                else
                {
                    std::cerr << "Failed to listen on socket... " << '\n';
                }
            }
            else
            {
                std::cerr << "Failed to bind to local " << '\n';
            }
        }
        else
        {
            std::cerr << "Failed to create socket " << '\n';
            continue;
        }
    }

    // Free the linkedlist
    freeaddrinfo(res);

    return listener_fd;
}

int Socket::new_connection(int listener_fd)
{

    struct sockaddr_storage client_addr;
    struct sockaddr *clientaddrptr = (struct sockaddr *)&client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    // client address -> inet_ntop changes this from network ip to presentable so we can actually read it
    char client_addr_s[INET6_ADDRSTRLEN];

    // Creating a connection socket
    int connecfd = accept(listener_fd, clientaddrptr, &client_addr_size);

    if (connecfd >= 0)
    {
        inet_ntop(client_addr.ss_family, get_in_addr(clientaddrptr), client_addr_s, sizeof client_addr_s);
        std::cout << "Incoming connection from: " << client_addr_s << '\n';
    }

    return connecfd;
}

int Socket::set_nonblock()
{

    int flags = fcntl(m_fd, F_GETFL);
    if (flags == -1)
    {
        //  failed to configure socket correctly
        return -1;
    }

    return fcntl(m_fd, F_SETFL, flags | O_NONBLOCK);
}
