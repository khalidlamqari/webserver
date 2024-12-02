/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   polling.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 15:08:50 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/02 17:52:11 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <signal.h>

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
    int                         n_events = 0;

    while (true)
    {
        for ( int i = 0 ; i < n_events; ++i)
        {
            ft_memset(&events[i], 0, sizeof(events[i]));
        }
        // ft_memset(&events, 0, sizeof(events));
        
        if ((n_events = kevent(kqueue_fd, NULL, 0, events, 1000, NULL)) == -1)
        {
            close_sockets_on_error(activeListners);
            close_client_sockets_on_error(activeClients);
            std::cout << "Webserv : kevent(1) failed, reason : " << std::endl;
            throw std::runtime_error(std::string("Webserv : kevent(1) failed, reason : ") + strerror(errno));
        }
    // try{

        for (int i = 0; i < n_events; i++)
        {
            if ( events[i].udata == NULL )
                continue;
            if((((Socket *) events[i].udata) && ((events[i].flags & EV_EOF || events[i].flags & EV_ERROR) || (events[i].fflags & EV_EOF || events[i].fflags & EV_ERROR))) && (((Socket *) events[i].udata)->get_type() != 'P'))
            {
                if ((((Socket *) events[i].udata)->get_type() != 'G'))
                {
                    close (events[i].ident);
                    std::cout << events[i].ident << " is closed" << std::endl;
                    delete_client(activeClients, events[i].ident);
                    // break;
                }
            }
            else if ( ((Socket *) events[i].udata) && ((Socket *) events[i].udata)->get_type() == 'L')
            {
                ListenerSocket * listener = (ListenerSocket *) events[i].udata;
                try
                {
                    accept_client_connection(listener, kqueue_fd, activeClients);
                }
                catch(const std::exception& e)
                {
                    std::cout << "accept_client_connection" << std::endl;
                    close_sockets_on_error(activeListners);
                    close_client_sockets_on_error(activeClients);
                    throw ;
                }
            }
            else if ( ((Socket *) events[i].udata)->get_type() == 'C' && events[i].filter == EVFILT_READ)
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
                    std::cout << "handle_client_request" << std::endl;
                    close_sockets_on_error(activeListners);
                    close_client_sockets_on_error(activeClients);
                    throw ;
                }
            }
            else if ( ((Socket *) events[i].udata)->get_type() == 'C' && events[i].filter == EVFILT_WRITE && ((ClientSocket *)(events[i].udata))->request->isReady() )
            {
                ClientSocket  * client_info = (ClientSocket *) events[i].udata;
                try
                {
                    respond_to_client(client_info, kqueue_fd, n_events, events );
                    if ( client_info->response->end_of_response() )
                    {
                        std::cout << "end of response " << std::endl;
                        if ( client_info->request->get_headers().find("CONNECTION")->second == "close" ) // should check if the responde with error
                        {
                            close(events[i].ident);
                            delete_client(activeClients, events[i].ident);
                            std::cout << "connection closed" << std::endl;
                        }
                        else
                        {
                            if ((client_info->cgiprocess && client_info->cgiprocess->response->get_exit_stat() != -1) || !client_info->cgiprocess)
                            {
                                if (client_info->cgiinfo ) {
                                    client_info->cgiinfo->request = NULL;
                                    client_info->cgiinfo->response = NULL;
                                }
                                if ( client_info->cgiprocess )
                                {
                                    client_info->cgiprocess->request = NULL;
                                    client_info->cgiprocess->response = NULL; 
                                }
                                delete client_info->request;
                                delete client_info->response;
                                client_info->request = new Request();
                                delete client_info->cgiinfo;
                                delete client_info->cgiprocess;
                                client_info->cgiinfo = NULL;
                                client_info->cgiprocess = NULL;
                                client_info->response = NULL;
                                client_info->request->set_ClientSocket(client_info);
                                switch_interest(client_info, kqueue_fd, EVFILT_WRITE, EVFILT_READ); // Interest is gonna be switched only if the response has been entirely sent.
                                // break;
                            }
                        }
                    }
                }
                catch(const std::exception& e)
                {
                    std::cout << "respond_to_client" << std::endl;
                    close_sockets_on_error(activeListners);
                    close_client_sockets_on_error(activeClients);
                    throw ;
                }
            }
            else if ( ((Socket *) events[i].udata)->get_type() == 'P' && events[i].filter == EVFILT_PROC && (events[i].fflags & NOTE_EXIT))
            {
                CgiProcess  * process_info = (CgiProcess *) events[i].udata;
                if (process_info && process_info->response )
                {
                    std::cout << "process_info->response->get_process_id() ; " << process_info->response->get_process_id() << std::endl;
                    waitpid(process_info->response->get_process_id(), NULL, 0);
                    process_info->response->set_exit_stat(0);
                    // break;
                }
                // delete process_info;
                // events[i].udata = NULL;
                // process_info->response->set_end_of_response(true);
                
            }
        }
    // }
    // catch(...)
    // {
    //     std::cout << "some error " << std::endl;
    // }
    }
    
}