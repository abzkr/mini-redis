#ifndef SOCKETS_H
#define SOCKETS_H

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT "6379"
#define BACKLOG 10

#define MAX_CONNECTIONS 10

void *get_in_addr(struct sockaddr *ap);

// Constructing an object doesn't guarantee an open socket, so error check in server loop
class Socket
{
public:
    Socket(int family, int socktype, int flags)
        : m_fd(start_listener(family, socktype, flags)) {}

    explicit Socket(int listener_fd)
        : m_fd(new_connection(listener_fd)) {}

    Socket &operator=(const Socket &) = delete;
    Socket(const Socket &) = delete;

    Socket(Socket &&sock) noexcept;

    int getFd() const { return m_fd; }
    int start_listener(int family, int socktype, int flags);
    int set_nonblock();
    int new_connection(int listener_fd);

    ~Socket()
    {
        if (m_fd >= 0) // check fd is open
        {
            std::cout << "Closing now" << '\n';
            close(m_fd);
        }
    }

private:
    int m_fd{};
};

#endif