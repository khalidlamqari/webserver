
#include "Server.hpp"

Server::Server() {}

Server::~Server() {}

void    Server::setup(const HttpContext & http_config)
{
    socketManager.create_listeners(http_config);
    kqueueManager.create_kqueue();
    kqueueManager.register_listeners_in_kqueue(socketManager.get_listeners());
}

void    Server::accept_client_connection(ListenerSocket * listener)
{
    ClientSocket    *new_client = NULL;

    try {                                                       // TODO : TEST THE PROTECTION
        new_client = new ClientSocket();
        int             client_sock_fd;

        if ((client_sock_fd = accept(listener->get_ident(), NULL, 0)) == -1)
            throw std::runtime_error("Accept failed!");

        fcntl(client_sock_fd, F_SETFL, O_NONBLOCK);

        new_client->set_ident(client_sock_fd);
        new_client->set_servers(listener->get_servers());
        new_client->set_request(new Request());
        new_client->set_response(new Response(*new_client));

        if (kqueueManager.register_event_in_kqueue(new_client, EVFILT_READ))
            throw std::runtime_error("New client event registration failed!");
    }
    catch( const std::exception &e ) {
        return (delete new_client);
    }

    socketManager.add_client(new_client);
}


void    Server::handle_cgi_process(CgiProcess * cgiProc)
{
    Response * response = cgiProc->get_response();
    int status = 0;

    waitpid(cgiProc->get_ident(), &status, WNOHANG);
    
    if ((WIFEXITED(status)))
    {
        status = WEXITSTATUS(status);
        response->set_exit_stat(status);
        if (status != 0)
            response->set_status(500);
    }
    else
    {
        response->set_status(500);
        response->set_exit_stat(1);
    }
    cgiProc->markAsStoped();
}

void    Server::handle_client_event(struct kevent & event)
{
    // Checking if the socket has been closed for reading (EOF) or if any error has occurred on the socket.
    if (event.flags & EV_EOF || event.fflags & EV_ERROR) 
    {
        if (LOG_ENABLED)
            std::cout   << CYAN << "[ " << get_time(time(0))
                    << " ] \"Client disconected!" << RESET << std::endl;
        
        socketManager.delete_client(event.ident);
        return ;
    }
    ClientSocket  * client_info = static_cast<ClientSocket *> (event.udata);

    if (event.filter == EVFILT_READ)
    {
        handle_client_request(client_info);
        if (client_info->get_request()->isBadRequest() || client_info->get_request()->isReady())
        {
            if (kqueueManager.switch_interest(client_info, EVFILT_READ, EVFILT_WRITE))
                socketManager.delete_client(event.ident);
        }
    }
    else if (event.filter == EVFILT_WRITE)
    {
        try {
            respond_to_client(client_info ,kqueueManager);
        }
        catch(const std::exception& e) {
            socketManager.delete_client(event.ident);
            return ;
        }
        if (client_info->get_response()->end_of_response() && !client_info->get_request()->isPersistent())
        {
            print_log(*(client_info->get_response()), true);
            socketManager.delete_client(event.ident);
    
        }
        else if (client_info->get_response()->end_of_response())
        {
            print_log(*(client_info->get_response()), false);
            setup_request_response(client_info, socketManager);
            if (kqueueManager.switch_interest(client_info, EVFILT_WRITE, EVFILT_READ))
                socketManager.delete_client(event.ident);
        }
    }
}

int global_status(int sts, bool set)
{
    static int status = 200;

    if (set)
        status = sts;
    return status;
}

void    Server::handle_cgi_output(struct kevent & event)
{
    CgiPairSocket * pairSocket = static_cast<CgiPairSocket *> (event.udata);

    Response* rsp = pairSocket->get_response();    
    rsp->read_cgi_output();
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
            if (checkBrokenIdents(events[i].ident, "CHECK"))
                continue ;

            switch ((static_cast<KqueueIdent *> (events[i].udata)->get_type()))
            {
                case LISTENER_SOCK :
                {
                    accept_client_connection(static_cast<ListenerSocket *> (events[i].udata));
                    break ;
                }
                case CLIENT_SOCK :
                {
                    handle_client_event(events[i]);
                    break ;
                }
                case CGI_PAIR_SOCK :
                {
                    handle_cgi_output((events[i]));
                    break ;
                }
                case CHILD_ID :
                {  
                    handle_cgi_process(static_cast<CgiProcess *> (events[i].udata));
                    break ;
                }
            }
		}
        checkBrokenIdents(0, "CLEAR");
	}
}
