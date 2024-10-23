/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 10:28:23 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/23 22:52:50 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Server.hpp"
#include <cstring>
// class Request ;

Server::Server( ServerContext & server_context )
{
    this->server_context = server_context ;
    
    // this->server_fd = socket(AF_INET, SOCK_STREAM, 0); 
    // if (this->server_fd == -1)
    //     throw std::runtime_error("socket creation failure");

    // this->root  = "/Users/klamqari/Desktop/web";
    memset(&(this->address) , 0, sizeof(address)) ;
    
    
    this->addrlen                   = sizeof(address) ;
    this->address.sin_family        = AF_INET ;
    this->address.sin_addr.s_addr   = INADDR_ANY ;
    // this->address.sin_port          = htons(8000) ;

    for (int i = 0; i <= MAX_CLIENTS; ++i)
        this->fds[i].fd = -1; // Initialize

    this->number_of_ports = 0 ;
}

void    Server::init_server( void )
{
    

    this->number_of_ports = this->server_context.get_ports().size() ;

    std::vector<unsigned short>::iterator it = this->server_context.get_ports().begin() ;
    
    for ( int i = 0 ; i < this->number_of_ports ; i++ )
    {
        this->fds[i].fd = create_socket( *it ) ;
        this->fds[i].events = POLLIN ;
        it++ ;
    }

}

int Server::create_socket( int port )
{
    int serv_fd ;
    struct sockaddr_in address ;
    
    std::cout << "port : " << port << std::endl;
    serv_fd = socket( AF_INET, SOCK_STREAM, 0 ) ;
    if ( serv_fd == -1 )
        throw std::runtime_error(" can't create socket ") ;
        
    int opt = 1 ;
    // if ( setsockopt( serv_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt) ) != 0)
    if ( setsockopt( serv_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt) ) != 0 )
        throw std::runtime_error("error : setsockopt failed") ;
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
    
    if ( bind( serv_fd , ( struct sockaddr * )&address, sizeof(address) ) < 0 )
        throw std::runtime_error("error : bind failed") ;

    if ( listen ( serv_fd, MAX_CLIENTS ) < 0 )
        throw std::runtime_error("error : listen error") ;

    return ( serv_fd ) ;
}

void Server::run()
{
    this->init_server() ;

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
    for ( int i = 0 ; i < this->number_of_ports ; i++ )
    {
        if (this->fds[i].revents & POLLIN)  // Check if there is an incoming connection on the server socket
        {
            std::cout << "accept" << std::endl;
            this->new_socket = accept( this->fds[i].fd, (struct sockaddr*)&(this->address), (socklen_t*)&(this->addrlen) ) ;
            if ( this->new_socket < 0 )
                return ( true ) ;

            for ( int j = this->number_of_ports; i <= MAX_CLIENTS; ++j )
            {
                if ( this->fds[j].fd == -1 )
                {
                    this->fds[j].fd = this->new_socket ;
                    this->fds[j].events = POLLIN ;
                    break ;
                }
            }
        }
        
    }
    return ( false );
}

void Server::handl_each_client_socket()
{
    for (int i = this->number_of_ports ; i <= MAX_CLIENTS; ++i)
    {
        if (this->fds[i].fd != -1 && (this->fds[i].revents & POLLIN))
        {
            char message[BUFFER_SIZE] = {0} ;
                std::cout << "read" << std::endl;
            if ( read(this->fds[i].fd, message, BUFFER_SIZE) == -1 )
            {
                // throw std::runtime_error("read failure ") ;
                close(this->fds[i].fd) ;
                this->fds[i].fd = -1 ;
            }
            else
            {
                
                Request request ;
                // std::cout << message << std::endl;
                // here we will handle request
                try
                {
                    request.setMessage ( message ) ;
                    request.parseMessage () ;
                }
                catch(int error_num)
                {
                    std::cout << "ERROR : " << error_num << std::endl;
                }
                

                std::cout << "method         " << request.method << std::endl ;
                std::cout << "request_target " << request.request_target << std::endl ;
                std::cout << "HTTP_version   " << request.HTTP_version << std::endl ;

                for (std::map<std::string, std::vector<std::string> >::iterator it = (request.headers).begin(); it != (request.headers).end(); ++it)
                {
                    std::cout    << "key   : " << it->first << std::endl ;

                    for (std::vector<std::string>::iterator j = (it->second).begin() ; j != (it->second).end() ; j++ )
                    {
                        std::cout << "value : " << *j << std::endl ;
                    }
                    std::cout << "        ----------------------------------------          " << std::endl ;
                }

                // here we will handle response
                
                int fd ;
                char response[2024] ;
                std::string error_404 = "<h1>error 404, page not found !!</h1>" ;
                if ( request.request_target == "/" )
                {
                    fd = open("index.html", O_RDONLY) ;
                    ssize_t size = read(fd, response, 2024) ;
                    response[size] = '\0' ;
                    close(fd) ;
                }
                else
                {
                    fd = open((root + request.request_target).c_str(), O_RDONLY) ;
                    ssize_t size = read(fd, response, 2024) ;
                    response[size] = '\0' ;
                    close(fd) ;

                }
                std::string response1 = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
                // const char * response1 = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<h1 style='color:red;'>Hello from C++ server!</h1>";
                if ( send(this->fds[i].fd, response1.c_str(), response1.length(), 0) == -1 )
                    throw std::runtime_error("send failure ") ;

                if (fd == -1)
                        send(this->fds[i].fd, error_404.c_str(), error_404.length(), 0);
                else
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
    this->closeServer();
}
