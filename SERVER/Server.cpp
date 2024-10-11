/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 10:28:23 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/11 11:04:11 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Server.hpp"
#include <cstring>
class Request ;

Server::Server()
{
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->server_fd == -1) 
        throw std::runtime_error("socket creation failure");
    memset(&(this->address) , 0, sizeof(address));
    this->addrlen = sizeof(address) ;
    this->address.sin_family = AF_INET ;
    this->address.sin_addr.s_addr = INADDR_ANY ;
    this->address.sin_port = htons(5000) ;

    std::cout << "server_fd " <<  server_fd << std::endl;
    if ( bind(this->server_fd, (struct sockaddr*)&(this->address), sizeof(this->address)) == -1)
    {
        if ( errno == EADDRINUSE )
            throw std::runtime_error("Port is already used ") ;
        else
            throw std::runtime_error("baind failure ") ;
    }

    if ( listen(this->server_fd, 3) == -1)
        throw std::runtime_error("listen failure ") ;


}

void Server::run()
{
    while (true) {
        
        this->new_socket = accept(server_fd, (struct sockaddr*)&(this->address), (socklen_t*)&(this->addrlen));

        char message[BUFFER_SIZE] = {0} ;

        if ( read(this->new_socket, message, BUFFER_SIZE) == -1 )
            throw std::runtime_error("read failure ") ;

        // std::cout << "Received: " << message << std::endl ;
        
        Request request(message) ;
        request.parseMessage() ;
        
        // std::cout << "method         " << request.method << std::endl;
        // std::cout << "request_target " << request.request_target << std::endl;
        // std::cout << "HTTP_version   " << request.HTTP_version << std::endl;
        
        // for (std::map<std::string, std::string>::iterator it = (request.headers).begin(); it != (request.headers).end(); ++it) 
        // {
        //     std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
        // }


        const char * response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<h1 style='color:red;'>Hello from C++ server!</h1>";


        if ( send(this->new_socket, response, strlen(response), 0) == -1 )
            throw std::runtime_error("send failure ") ;

        if ( close(this->new_socket) == -1 )
            throw std::runtime_error("close failure ") ;

    }
}

void Server::closeServer()
{
    if ( close (this->server_fd )  == -1 )
        throw std::runtime_error("close failure") ;
}


Server::~Server()
{
    // free 
}