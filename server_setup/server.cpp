/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 15:07:44 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/19 05:45:13 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

static bool already_binded(std::vector<struct ListenerSocket>& active_sockets, const ServerContext& server, struct in_addr host, unsigned short port)
{
    std::vector<struct ListenerSocket>::iterator  it = active_sockets.begin();
    std::vector<struct ListenerSocket>::iterator  end = active_sockets.end();

    for ( ; it != end; it++)
    {
        if ((it->host.s_addr == host.s_addr) && (it->port == port))
        {
            it->add_server(&server); // link the server to the socket already created for this host:port
            return true;
        }
    }
    return false;
}

struct addrinfo *my_get_addrinfo(const char * host)
{
    int             ec;             // error code;
    struct addrinfo *res;
    struct addrinfo hints;

    ft_memset(&hints, 0,sizeof(hints));
    hints.ai_family = AF_INET; // TODO : Try specifying AF_INET6 as address family hint and see if ipv4 addresses are gonna be returned as well.
    hints.ai_socktype = SOCK_STREAM;

    if ((ec = getaddrinfo(host, NULL, &hints, &res)) != 0)
    {
        if (ec == 8)
            err_throw((std::string("Unknown host : ") + host).c_str());
        err_throw(gai_strerror(ec));
    }
    return res;
}


void    initialize_sockets_on_port(struct addrinfo *list, std::vector<struct ListenerSocket>& active_sockets, const ServerContext& server, unsigned short port)
{
    int                 fd;
    const char          *cause = NULL;
    unsigned int        n_sock = 0;
    struct sockaddr_in * ip_access;
    int                 opt = 1; // TODO

    for (struct addrinfo *entry = list; entry ; entry = entry->ai_next)
    {
        if (already_binded(active_sockets, server, ((struct sockaddr_in *) entry)->sin_addr, port))
        {
            n_sock++;
            continue ;
        }

        fd = socket(entry->ai_family, entry->ai_socktype, entry->ai_protocol);
        if (fd == -1)
        {
            cause = "socket";
            continue ;
        }
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));

        ft_memset(ip_access, 0, sizeof(struct sockaddr_in));

        ip_access = (struct sockaddr_in *) entry->ai_addr;
        ip_access->sin_family = entry->ai_family;;
        ip_access->sin_port = htons(port);

        if (bind(fd, (struct sockaddr *) ip_access, sizeof(*ip_access)) == -1)
        {
            close(fd);
            cause = "bind";
            continue ;
        }

        // link the current server to the newlly created socket and add the socket to the active sockets list:
        {
            struct ListenerSocket   new_s;
            new_s.set_type('L');
            new_s.set_sock_fd(fd);
            new_s.host = ((struct sockaddr_in *)entry->ai_addr)->sin_addr;
            new_s.port = port;
            new_s.add_server(&server);

            active_sockets.push_back(new_s);

            std::cerr <<  inet_ntoa(((struct sockaddr_in *)(entry->ai_addr))->sin_addr) << ":" << ntohs(((struct sockaddr_in*)entry->ai_addr)->sin_port) << std::endl;
        }
        n_sock++;

        if (listen(fd, 128) == -1) // This tells the TCP/IP stack to start accept incoming TCP connections on the port the socket is binded to. 128 because in The mac im working on 128 is the maximum number of pending connections
            throw std::runtime_error("Webserv : listen() failed");
    }   

    if (n_sock == 0)
        throw cause;
}


void    setup_servers(const HttpContext& http_config, std::vector<struct ListenerSocket>&  activeListners)
{
    struct addrinfo *res;

    std::vector<ServerContext>::const_iterator serv_it = http_config.get_servers().begin();
    std::vector<ServerContext>::const_iterator end = http_config.get_servers().end();    

    for ( ; serv_it < end; serv_it++)
    {
        try
        {
            res = my_get_addrinfo(serv_it->get_host().c_str());
        }
        catch(const std::string& err)
        {
            close_sockets_on_error(activeListners);
            throw err;
        }

        {
            std::vector<unsigned short>::const_iterator ports_it = serv_it->get_ports().begin();
            std::vector<unsigned short>::const_iterator p_end = serv_it->get_ports().end();
            
            for ( ; ports_it < p_end; ports_it++ )
            {
                try
                {
                    initialize_sockets_on_port(res, activeListners, *serv_it, *ports_it);
                }
                catch(const char* err)
                {
                    close_sockets_on_error(activeListners);
                    throw err;
                }
            }
        }
        freeaddrinfo(res);
    }
}