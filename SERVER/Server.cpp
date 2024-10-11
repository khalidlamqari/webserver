/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 10:28:23 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/11 19:07:02 by klamqari         ###   ########.fr       */
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
    

    for (int i = 0; i <= MAX_CLIENTS; ++i)
        this->fds[i].fd = -1; // Initialize
}



void Server::run()
{
    // struct pollfd fds[MAX_CLIENTS + 1]; // +1 for the server socket
    
    if ( bind(this->server_fd, (struct sockaddr*)&(this->address), sizeof(this->address)) == -1)
    {
        if ( errno == EADDRINUSE )
            throw std::runtime_error("Port is already used ") ;
        else
            throw std::runtime_error("baind failure ") ;
    }

    if ( listen(this->server_fd, 3) == -1)
        throw std::runtime_error("listen failure ") ;
        
        
    this->fds[0].fd = this->server_fd;
    this->fds[0].events = POLLIN; // Monitor for incoming connections


    while (true)
    {
        
        this->poll_count = poll( this->fds, MAX_CLIENTS + 1, -1 ) ; // -1 means wait indefinitely
        if (this->poll_count < 0)
            throw std::runtime_error( strerror( errno ) );

        if ( this->check_incomming_connection_server() )
            continue ;

        this->handl_each_client_socket() ;

    }
}

bool Server::check_incomming_connection_server()
{
    if (this->fds[0].revents & POLLIN)// Check if there is an incoming connection on the server socket
    {
        this->new_socket = accept(this->server_fd, (struct sockaddr*)&(this->address), (socklen_t*)&(this->addrlen));
        if ( this->new_socket < 0 )
            return ( true ) ;
        
        for (int i = 1; i <= MAX_CLIENTS; ++i)
        {
            if ( this->fds[i].fd == -1 )
            {
                this->fds[i].fd = this->new_socket ;
                this->fds[i].events = POLLIN ;
                break ;
            }
        }
    }
    return ( false );
}


void Server::handl_each_client_socket()
{
    for (int i = 1; i <= MAX_CLIENTS; ++i)
    {
        if (this->fds[i].fd != -1 && (this->fds[i].revents & POLLIN))
        {
            char message[BUFFER_SIZE] = {0} ;

            if ( read(this->fds[i].fd, message, BUFFER_SIZE) == -1 )
            {
                // throw std::runtime_error("read failure ") ;
                close(this->fds[i].fd) ;
                this->fds[i].fd = -1 ;
            }
            else
            {
                // here we will handle request
                Request request(message) ;
                request.parseMessage() ;

                std::cout << "method         " << request.method << std::endl ;
                std::cout << "request_target " << request.request_target << std::endl ;
                std::cout << "HTTP_version   " << request.HTTP_version << std::endl ;
                
                for (std::map<std::string, std::vector<std::string> >::iterator it = (request.headers).begin(); it != (request.headers).end(); ++it)
                {
                    std::cout << "key : " << it->first << std::endl ;

                    for (std::vector<std::string>::iterator j = (it->second).begin() ; j != (it->second).end() ; j++ )
                    {
                        std::cout << "value : " << *j << std::endl ;
                    }
                    std::cout << "        ----------------------------------------          " << std::endl ;
                }


                // here we will handle response
                const char * response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<h1 style='color:red;'>Hello from C++ server!</h1>";
                if ( send(this->fds[i].fd, response, strlen(response), 0) == -1 )
                    throw std::runtime_error("send failure ") ;

                if ( close(this->fds[i].fd) == -1 )
                    throw std::runtime_error("close failure ") ;

            }
        }
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