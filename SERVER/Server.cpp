/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 10:28:23 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/05 15:35:21 by klamqari         ###   ########.fr       */
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

    this->requests.resize(MAX_CLIENTS + 1) ;
    for ( int i = 0 ; i < MAX_CLIENTS + 1; i++ )
    {
        this->requests[i].reuse() ;
        // this->requests[i].isReady = false ;
    }
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
            this->read_data_from_socket(i);
        }
        this->send_response( this->server_context , i ) ;
    }
}

void    Server::read_data_from_socket( int i )
{
    ssize_t             bytes_received = 1;
    std::vector<char>   buffer(BUFFER_SIZE);
    std::string message ;
    // std::vector<char>   buffer(10);

    if ( ! this->requests[i].isReady )
    {
        // while ( true )
        // {
            bytes_received = recv( this->fds[i].fd , buffer.data(), BUFFER_SIZE, MSG_DONTWAIT ) ; // MSG_DONTWAIT Makes the operation non-blocking.
            // std::cout << "bytes_received : " << bytes_received << std::endl ;

            if (bytes_received == -1)
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    // No more data to read
                    if ( ! this->requests[i].get_request_method().empty() && this->requests[i].content_length == this->requests[i].get_request_body().length())
                    {
                        this->requests[i].isReady = true ;
                        // break ;
                    }
                } else {
                    // Error occurred
                    std::cerr << "recv error: " << strerror(errno) << std::endl;
                    return ;
                }
            }
            // else if (bytes_received == 0) 
            //     break; // Connection closed by the client
                    
            
        
            if (bytes_received >= 0 && bytes_received < (ssize_t) buffer.size()) {
                buffer[bytes_received] = '\0';
            }
    
            if ( bytes_received > 0 && this->requests[i].get_request_method().empty() )
                this->requests[i].appendMessage( buffer.data(), bytes_received ) ;
            else 
                this->requests[i].appendTobody( buffer.data(), bytes_received ) ;
            
                


            // ------- 
            if ( this->requests[i].get_request_method().empty() &&  requests[i].getMessage().find("\r\n\r\n") != std::string::npos )
            {
                try
                {
                    this->requests[i].parseMessage() ;
                }
                catch ( int error )
                {
                    this->requests[i].setStatus( error ) ;
                    std::cout << "Error : " << error << std::endl ;
                }
            }
            if ( ! this->requests[i].get_request_method().empty() && this->requests[i].content_length == this->requests[i].get_request_body().length())
            {
                this->requests[i].isReady = true ;
                // break ;
            }

            
        // }
        
        // this->requests[i].isReady = true ;
        
        // try
        // {
        //     this->requests[i].parseMessage() ;
        // }
        // catch ( int error )
        // {
        //     error_page_number = error ;
        //     std::cout << "Error : " << error << std::endl;
        // }
        // std::cout << "buffer.data() " << buffer.data() << std::endl;
    }
    // this->print_request( this->requests[i] ) ;
}

void    Server::send_response(ServerContext & server_context , int i)
{
    std::string msg ;

    if ( this->requests[i].isReady )
    {
        print_request( this->requests[i] ) ;
        std::cout << ".......................... i = " << i  << std::endl ;
        Response response( server_context, this->requests[i] ) ;

        while ( true ) // for testting
        {
            std::cout << "send " << std::endl;
            msg = response.getResponse() ;

            ssize_t size = send( this->fds[i].fd, msg.c_str(), msg.length(), 0 ) ;
            if ( size == -1 )
                throw std::runtime_error("send failure ") ;

            if ( (response.tranfer_encoding() && response.getPageStream().gcount() == 0) ||  ( ! response.tranfer_encoding() && response.end_of_response() ))
                break ;
        }

        if ( close(this->fds[i].fd) == -1 )
        {
            this->fds[i].fd = -1 ;
            throw std::runtime_error("close failure ") ;
        }

        this->fds[i].fd = -1 ;
        this->requests[i].reuse() ;

        // this->requests[i].isReady = false ;
        // this->requests[i].method = "";

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

/* FOR Testting */
void Server::print_request(Request & request)
{
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
    
    std::cout << "body length   " << request.body.length() << std::endl ;
    std::cout << "status   " << request.getStatus() << std::endl ;
}



/*

( 
  echo -e "POST /upload HTTP/1.1\r\nHost: 127.0.0.1:8888\r\nContent-Type: image/png\r\nContent-Length: $(stat -f%z img.png)\r\nConnection: close\r\n\r\n"
  cat img.png 
) | nc 127.0.0.1 8888

*/