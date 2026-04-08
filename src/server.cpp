#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT "6379"
#define BACKLOG 10

void* get_in_addr (struct sockaddr* ap) {
    if(ap->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) ap) ->sin_addr);
    }
    else{
        return &(((struct sockaddr_in6*) ap) ->sin6_addr);
    }
}

int server() {

    struct addrinfo hints;
    struct addrinfo *res;
    
    struct sockaddr_storage client_addr;
    struct sockaddr* clientaddrptr = (struct sockaddr* )&client_addr;
    
    socklen_t client_addr_size = sizeof(client_addr);
    char client_addr_s[INET6_ADDRSTRLEN];
    char buff [20];
    ssize_t lenbuff = sizeof buff;
    
    // error codes
    int recv_status;
    int send_status;
    int exit_status;
    
    int sockfd, connecfd;
    
    
    // Ensure that its empty - fill with 0s
    memset(&hints, 0, sizeof(hints));

    // How we want our socket configurations 
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Error checking getaddrinfo
    if((exit_status = getaddrinfo(NULL, PORT, &hints ,&res)) != EXIT_SUCCESS ){
        std::cerr << stderr << ' ' << gai_strerror(exit_status);
    }
    
    struct addrinfo *ptr;

    // Loop to find a valid entry in the linked list and error checking each system call
    for(ptr = res; ptr != nullptr; ptr = ptr->ai_next){
        if((sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) >= 0){
            
            int yes = 1;
            setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

            if(bind(sockfd, ptr->ai_addr, ptr->ai_addrlen) >= 0){
                if (listen(sockfd, BACKLOG) >= 0){
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

    // Creating sockets for each new connection
    while((connecfd = accept(sockfd, clientaddrptr, &client_addr_size)) >= 0){
                
        inet_ntop(client_addr.ss_family, get_in_addr(clientaddrptr), client_addr_s, sizeof client_addr_s);
        std::cout << "Incoming connection from: " << client_addr_s << '\n';

        while((recv_status = recv(connecfd, &buff, lenbuff, 0)) > 0){
            std::cout << buff << '\n';
            if( (send_status = send(connecfd, &buff, lenbuff, 0)) >= 0){
                std::cout << "Message sent to client!" << '\n';
            }
            else if (send_status < 0){
                std::cerr << "Error sending data: " << send_status << '\n';
            }
        }
        if(recv_status == 0){
            std::cout << "Client closed their connection to the server " << '\n';
        }
        else if(recv_status < 0){
            std::cerr << "Error recieving data: " << recv_status << '\n';
        }

        
    }
    
    return 0;
}


int main () {
    return server();

}
