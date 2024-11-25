/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:08:35 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/25 05:53:07 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "../Request/request_parse.hpp"
#include "../Contexts/HttpContext.hpp"
#include "../Utils/utils.hpp"
#include "../macros.hpp"
#include "../response/Response.hpp"
/// include only needed hraders ! 

#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <arpa/inet.h>
#include <map>
#include <netdb.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>


#define TMOUT_SEC 1 
#define TMOUT_NSEC 2000 
#define READ_BUFFER_SIZE 11024
class Response;
struct Socket
{
    void    set_type( char type )
    {
        this->type = type;
    }

    char    get_type( void )
    {
        return this->type;
    }

    int     get_sock_fd( void )
    {
        return this->sock_fd;
    }

    void    set_sock_fd( int fd )
    {
        this->sock_fd = fd;
    }

    std::vector<const ServerContext*>&   get_servers( void )
    {
        return this->servers;
    }

    void    set_servers( std::vector<const ServerContext*>& servers )
    {
        this->servers.insert(this->servers.end(), servers.begin(), servers.end());
    }

    void    add_server( const ServerContext* new_server )
    {
        this->servers.push_back(new_server);
    }

private :

    int                                 sock_fd;
    char                                type;       // 'L' for ListenerSocket | 'C' for client socket
    std::vector<const ServerContext*>   servers;
};

struct ListenerSocket : public Socket
{
    struct in_addr                      host;       // In the host representation
    unsigned short                      port;       // In the host representation

    ListenerSocket& get_instance( void )
    {
        return *this;
    }
};

struct ClientSocket : public Socket
{
    Request * request;
    Response * response;
    ClientSocket& get_instance( void )
    {
        return *this;
    }
};

struct CgiProcess : public Socket
{
    Request * request;
    Response * response;
};

/*                              Sockets                              */
void    setup_servers(const HttpContext& http_config, std::vector<struct ListenerSocket>&  activeListners);

/*                              Kqueue                              */
void    register_listeners_in_kqueue(int kqueue_fd, std::vector<struct ListenerSocket> & activeListners);
void    switch_interest(ClientSocket* client_info, int kqueue_fd,short old_filter, short new_filter);
void    poll_events(int kqueue_fd, std::vector<struct ListenerSocket> & activeListners);
void    register_socket_in_kqueue(int kqueue_fd, Socket * sock_data, short filter);
int     create_kqueue( void );

/*                              Client Management                              */
void    accept_client_connection(ListenerSocket *listener, int kqueue_fd, std::vector<ClientSocket*>& activeClients);
void    delete_client(std::vector<ClientSocket *>& activeClients, int fd);
void    handle_client_request(ClientSocket* client_info);
void    respond_to_client(ClientSocket* client_info, int kqueue_fd, int n_events, struct kevent * events);



#endif