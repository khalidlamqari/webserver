/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 15:07:44 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/21 11:45:12 by klamqari         ###   ########.fr       */
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
        throw std::runtime_error("accept() failed!"); // TODO

    fcntl(client_sock_fd, F_SETFL, O_NONBLOCK); // TODO

    new_client->set_ident(client_sock_fd);
    new_client->set_servers(listener->get_servers());
    new_client->set_request(new Request()); // TODO

    socketManager.add_client(new_client);
    kqueueManager.register_socket_in_kqueue(new_client, EVFILT_READ);
}

// ADD NEW

void    get_exit_status(CgiProcess  & process_info)
{
    Response * response = process_info.get_response();
    int status = 0;

    waitpid(response->get_process_id(), &status, 0);
    response->set_exit_stat(WEXITSTATUS(status));
    if (WEXITSTATUS(status) != 0)
        response->set_status(500);
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
                    kqueueManager.switch_interest(client_info, EVFILT_READ, EVFILT_WRITE);
            }
            else if (((KqueueIdent *) events[i].udata)->get_type() == CLIENT_SOCK && events[i].filter == EVFILT_WRITE)
            {
                respond_to_client((ClientSocket *) events[i].udata, socketManager ,kqueueManager);
            }
            else if ( ((KqueueIdent *) events[i].udata)->get_type() == CHILD_ID && events[i].filter == EVFILT_PROC && (events[i].fflags & NOTE_EXITSTATUS) )
            {
                get_exit_status(*((CgiProcess *) events[i].udata));
            }
            else if ( ((KqueueIdent *) events[i].udata)->get_type() == CGI_PAIR_SOCK && events[i].filter == EVFILT_READ)
            {
                ((CgiPairSocket *) events[i].udata)->get_response()->read_cgi_output();
            }
		}
	}
}
