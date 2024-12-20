/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 15:07:44 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/20 21:37:44 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
// #include "../response/Response.hpp"

Server::Server()
{
    
}

Server::~Server()
{
    
}

void    Server::setup(const HttpContext & http_config)
{
    socketManager.create_listeners(http_config);
    kqueueManager.create_kqueue();
    kqueueManager.register_listeners_in_kqueue(socketManager.get_listeners());
}

void    Server::accept_client_connection(ListenerSocket * listener)
{
    ClientSocket    *new_client = new ClientSocket();
    int             client_sock_fd;

    if (!new_client) // TODO : spread this protections on all possible cases where allocation might fail
        return ;

    if ((client_sock_fd = accept(listener->get_ident(), NULL, 0)) == -1)
        throw std::runtime_error("accept() failed!");

    fcntl(client_sock_fd, F_SETFL, O_NONBLOCK); // TODO

    new_client->set_ident(client_sock_fd);
    new_client->set_servers(listener->get_servers());
    new_client->set_request(new Request());

    socketManager.add_client(new_client);
    kqueueManager.register_socket_in_kqueue(new_client, EVFILT_READ);
}

void    Server::start()
{
    struct kevent	events[EVENT_QUEUE_CAPACITY];
    int				n_events;

	while (true)
	{
	    memset(&events, 0, sizeof(events));
		n_events = kqueueManager.poll_events(events, EVENT_QUEUE_CAPACITY);

		for (int i = 0; i < n_events; i++)
		{     
			if (((events[i].flags & EV_EOF || events[i].flags & EV_ERROR)
				|| (events[i].fflags & EV_EOF || events[i].fflags & EV_ERROR))
                && ((KqueueIdent *) events[i].udata)->get_type() == CLIENT_SOCK) // TODO : this shouldn t be done on a non socket ident type.
			{
				std::cerr << "Client disconected!" << std::endl;
                socketManager.delete_client(events[i].ident);
			}
			else if (((KqueueIdent *) events[i].udata)->get_type() == LISTENER_SOCK)
			{
                ListenerSocket * listener = (ListenerSocket *) events[i].udata;
                accept_client_connection(listener);
			}
            else if (((KqueueIdent *) events[i].udata)->get_type() == CLIENT_SOCK && events[i].filter == EVFILT_READ)
            {
                ClientSocket  * client_info = (ClientSocket *) events[i].udata;
                handle_client_request(client_info);
                if (client_info->get_request()->isBadRequest() || client_info->get_request()->isReady())
                {
                    kqueueManager.switch_interest(client_info, EVFILT_READ, EVFILT_WRITE);
                }
            }
            else if (((KqueueIdent *) events[i].udata)->get_type() == CLIENT_SOCK && events[i].filter == EVFILT_WRITE)
            {
                ClientSocket  * client_info = (ClientSocket *) events[i].udata;
                respond_to_client(client_info, kqueueManager.get_kqueue_fd());
                Response * response = client_info->get_response();
                /* checking if responding is done */
                if ( (!response->is_cgi() && response->end_of_response()) || (response->is_cgi() && response->get_exit_stat() != -1 && response->end_of_response()) )
                {
                    std::cout << "end of response " << std::endl;
                    if ( response->get_connection() == "close")
                    {
                        socketManager.delete_client(events[i].ident);
                    }
                    else
                    {
                        client_info->delete_request();
                        client_info->delete_response();
                        client_info->set_request(new Request());
                        kqueueManager.switch_interest(client_info, EVFILT_WRITE, EVFILT_READ);
                    }
                }
            }
            else if ( ((KqueueIdent *) events[i].udata)->get_type() == CHILD_ID && events[i].filter == EVFILT_PROC && (events[i].fflags & NOTE_EXITSTATUS) )
            {
                std::cout << "exited" << std::endl;
                CgiProcess  * process_info = (CgiProcess *) events[i].udata;
                if (process_info && process_info->get_response())
                {
                    Response * response = process_info->get_response();
                    int status = 0;
                    waitpid(response->get_process_id(), &status, 0);
                    response->set_exit_stat(WEXITSTATUS(status));
                    if (WEXITSTATUS(status) != 0)
                        response->set_status(500);
                    std::cout << "status : " << WEXITSTATUS(status) << std::endl;
                }
            }
            else if ( ((KqueueIdent *) events[i].udata)->get_type() == CGI_PAIR_SOCK && events[i].filter == EVFILT_READ)
            {
                CgiPairSocket  * sock_info = (CgiPairSocket *) events[i].udata;
                if (sock_info->get_response())
                {
                    sock_info->get_response()->read_cgi_output();
                }
            }
		}
	}
}
