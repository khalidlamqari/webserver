#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#define BUFFER_SIZE 30000

int main() {

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    int addrlen = sizeof(address) ;
    int new_socket ;

    address.sin_family = AF_INET ;
    address.sin_addr.s_addr = INADDR_ANY ;
    address.sin_port = htons(8080) ;
    try {
    if ( bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1)
    {
        // should free memory first
        throw std::runtime_error("bind failed ") ;
    }

    if ( listen(server_fd, 3) == -1)
    {
        // should free memory first
        throw std::runtime_error("listen failed ") ;
    }

    while (true) {

        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

        char buffer[BUFFER_SIZE] = {0} ;

        if ( read(new_socket, buffer, BUFFER_SIZE) == -1 )
        {
            // should free memory first
            throw std::runtime_error("read failed ") ;
        }
    
        std::cout << "Received: " << buffer << std::endl;

        const char * hello = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<h1 style='color:red;'>Hello from C++ server!</h1>";

        if ( send(new_socket, hello, strlen(hello), 0) == -1 )
        {
            // should free memory first
            throw std::runtime_error("send failed ") ;
        }

        if ( close(new_socket) == -1 )
        {
            //should free memory 
            throw std::runtime_error("close failed ") ;
        }

    }

    }
    catch (const std::exception & e )
    {
        std::cout << e.what() << std::endl ;
    }
    
    if ( close(server_fd) == -1 )
        return ( 1 ) ; 

    return 0;
}