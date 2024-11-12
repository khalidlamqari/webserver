/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 10:23:18 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/08 21:57:50 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# ifndef SERVER_HPP
# define SERVER_HPP

# include "../includes/main.hpp"
# include "../config_file_parsing/HttpContext.hpp"
#include "../response/Response.hpp"

typedef struct s_request
{
    bool isReady;
    Request request ;
}   t_request ;

class Server
{
    
    private :

        int                 server_fd ;

        struct sockaddr_in  address ;

        int                 addrlen ;

        int                 new_socket ;

        int                 poll_count ;

        struct pollfd       fds[MAX_CLIENTS + 1] ;
        
        std::deque < Request >     requests ;

        int                 number_of_ports ;

        std::string         root ;

        ServerContext       server_context ;

        void                init_server( void ) ;
        int                 create_socket( int port ) ;
        bool                check_incomming_connection_server() ;
        void                handl_each_client_socket() ;
        
        void                read_data_from_socket( int i );
        
        void                send_response( ServerContext & server_context, int i ) ;


    public :
        
        /* Setters */
        
        // void setServerContext( ServerContext & server_context ) ;
        
        
        
        Server( ServerContext & server_context ) ;


        ~Server() ;
    
        void                run() ;
        void                closeServer () ;
        
        /* For Testting */
        void print_request( Request & request) ;
        
};

#endif