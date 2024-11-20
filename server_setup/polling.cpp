/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   polling.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 15:08:50 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/20 05:07:03 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void    print_result(Request & client_request)
{
    if (client_request.isBadRequest())
    {
        std::cout << "Bad request!" << std::endl;
    }
    else
    {
        client_request.print_files();
    }
}

void    poll_events(int kqueue_fd, std::vector<struct ListenerSocket> & activeListners)
{
    std::vector<ClientSocket*>  activeClients;
    struct kevent               events[1000]; // TODO
    int                         n_events;

    while (true)
    {
        ft_memset(events, 0, sizeof(events));
        
        if ((n_events = kevent(kqueue_fd, NULL, 0, events, 1000, 0)) == -1)
        {
            close_sockets_on_error(activeListners);
            close_client_sockets_on_error(activeClients);
            throw std::runtime_error(std::string("Webserv : kevent(1) failed, reason : ") + strerror(errno));
        }

        for (int i = 0; i < n_events; i++)
        {
            if((events[i].flags & EV_EOF || events[i].flags & EV_ERROR)
             || (events[i].fflags & EV_EOF || events[i].fflags & EV_ERROR))
            {
                std::cout << "Client disconected!" << std::endl;
                delete_client(activeClients, events[i].ident);
                close(events[i].ident);
            }
            else if (((Socket *) events[i].udata)->get_type() == 'L')
            {
                ListenerSocket * listener = (ListenerSocket *) events[i].udata;

                try
                {
                    accept_client_connection(listener, kqueue_fd, activeClients);
                }
                catch(const std::exception& e)
                {
                    close_sockets_on_error(activeListners);
                    close_client_sockets_on_error(activeClients);
                    throw ;
                }                
            }
            else if (((Socket *) events[i].udata)->get_type() == 'C' && events[i].filter == EVFILT_READ)
            {
                ClientSocket  * client_info = (ClientSocket *) events[i].udata;

                try
                {
                    handle_client_request(client_info);
                    if (client_info->request->isBadRequest() || client_info->request->isReady())
                    {
                        Request&    client_request = *(client_info->request);
                        print_result(client_request);
                        switch_interest(client_info, kqueue_fd, EVFILT_READ, EVFILT_WRITE); // Interest is gonna be switched only if the request has been entirely rood
                    }
                }
                catch(const std::exception& e)
                {
                    close_sockets_on_error(activeListners);
                    close_client_sockets_on_error(activeClients);
                    throw ;
                }
            }
            else if (((Socket *) events[i].udata)->get_type() == 'C' && events[i].filter == EVFILT_WRITE)
            {
                ClientSocket  * client_info = (ClientSocket *) events[i].udata;

                try
                {
                    respond_to_client(client_info);
                    switch_interest(client_info, kqueue_fd, EVFILT_WRITE, EVFILT_READ); // Interest is gonna be switched only if the response has been entirely sent.
                }
                catch(const std::exception& e)
                {
                    close_sockets_on_error(activeListners);
                    close_client_sockets_on_error(activeClients);
                    throw ;
                }
            }
        }
    }   
}