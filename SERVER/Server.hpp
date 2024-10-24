/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 10:23:18 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/24 18:07:53 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# ifndef SERVER_HPP
# define SERVER_HPP

# include "../includes/main.hpp"
# include "../request/Request.hpp"
# include "../response/Response.hpp"
# include "../config_file_parsing/HttpContext.hpp"

class Server
{
    
    private :

        int                 server_fd ;

        struct sockaddr_in  address ;

        int                 addrlen ;

        int                 new_socket ;

        int                 poll_count ;

        struct pollfd       fds[MAX_CLIENTS + 1] ;

        int                 number_of_ports ;
        
        std::string         root ;
        
        ServerContext       server_context ;

        void                init_server( void ) ;
        int                 create_socket( int port ) ;
        bool                check_incomming_connection_server() ;
        void                handl_each_client_socket() ;
        
        void                send_response( ServerContext & server_context, Request & request, int index , int error_page_number) ;
        

    public :
        
        /* Setters */
        
        // void setServerContext( ServerContext & server_context ) ;
        
        
        
        Server( ServerContext & server_context ) ;


        ~Server() ;
    
        void                run() ; // 
        void                closeServer () ;
        
        /* For Testting */
        void print_request( Request & request) ;
        
};

#endif