/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 10:23:18 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/14 16:44:11 by klamqari         ###   ########.fr       */
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

        bool check_incomming_connection_server() ;
        void handl_each_client_socket() ;
        std::string root;

    public :
        Server() ;

        ~Server() ;
    
        void run() ; // 
        void closeServer () ;
        
};
#endif