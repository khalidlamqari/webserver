/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 10:23:18 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/14 20:20:03 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# ifndef SERVER_HPP
# define SERVER_HPP

# include "../includes/main.hpp"
# include "../request/Request.hpp"

class Server
{
    
    private :
    
        int                 server_fd ;
        struct sockaddr_in  address ;
        int                 addrlen ;
        int                 new_socket ;
        int                 poll_count ;
        struct pollfd       fds[MAX_CLIENTS + 1];
        std::string         root;

        bool                check_incomming_connection_server() ;
        void                handl_each_client_socket() ;


    public :
        Server() ;

        ~Server() ;
    
        void                run() ; // 
        void                closeServer () ;
        
};
#endif