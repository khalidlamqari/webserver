/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 11:37:00 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/25 07:20:59 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void delete_client(std::vector<ClientSocket *>& activeClients, int fd)
{
    std::vector<ClientSocket *>::iterator it = activeClients.begin();
    std::vector<ClientSocket *>::iterator end = activeClients.end();

    for ( ; it != end; it++)
    {
        if ((*it)->get_sock_fd() == fd) // TODO : why dont i store clients in a map instead of a vector like that the key will be the client fd so i dont have to loop over all clients to delete on of them
        {
            delete (*it)->request;
            delete (*it)->response;
            delete (*it);
            activeClients.erase(it);
            std::cout << "client deleted ! " << std::endl;
            return ;
        }
    }
}

void    accept_client_connection(ListenerSocket *listener, int kqueue_fd, std::vector<ClientSocket*>& activeClients)
{
    ClientSocket    *new_client = new ClientSocket;
    int             client_sock_fd;

    if ((client_sock_fd = accept(listener->get_sock_fd(), NULL, 0)) == -1)
        throw std::runtime_error("accept() failed!");

    fcntl(client_sock_fd, F_SETFL, O_NONBLOCK);

    new_client->set_type('C');
    new_client->set_sock_fd(client_sock_fd);
    new_client->set_servers(listener->get_servers());
    new_client->request = new Request();
    new_client->request->set_ClientSocket(new_client);

    activeClients.push_back(new_client);
    register_socket_in_kqueue(kqueue_fd, activeClients.back(), EVFILT_READ);
}

static  const ServerContext * get_server_context(ClientSocket * clientsocket)
{
    std::string host = clientsocket->request->get_headers().find("HOST")->second ;
    
    for (std::vector<const ServerContext*>::iterator i = clientsocket->get_servers().begin() ; i != clientsocket->get_servers().end() ; ++i)
    {
        std::vector<std::string>::const_iterator b = (*i)->get_server_names().begin();
        std::vector<std::string>::const_iterator e = (*i)->get_server_names().end();
        
        for ( std::vector<std::string>::const_iterator j = b ; j != e ; ++j)
        {
            if ( host == *j )
            {
                return ( *i ) ;
            }
        }
    }
    return (clientsocket->get_servers().front()) ;
}

void    determine_parsing_stage(Request & request, std::string & rcvdMsg)
{
    size_t      crlf_pos = rcvdMsg.find(CRLF);

    if ((!request.hasParsedStartLine() || !request.hasParsedHeaders()) && crlf_pos == std::string::npos)
        return request.storeUnparsedMsg(rcvdMsg); 

    rcvdMsg.insert(0, request.getUnparsedMsg());
    request.resetUnparsedMsg();
    crlf_pos = rcvdMsg.find(CRLF);

    if (!request.hasParsedStartLine())
    {
        parse_start_line(request, rcvdMsg.substr(0, crlf_pos));
        rcvdMsg.erase(0, crlf_pos + 2);
        return determine_parsing_stage(request, rcvdMsg);
    }

    if (!request.hasParsedHeaders())
    {
        parse_headers(request, rcvdMsg);
        return determine_parsing_stage(request, rcvdMsg);
    }

    if (request.hasParsedHeaders())
    {
        const ServerContext *  servercontext = get_server_context( request.get_ClientSocket() ); 
        // std::cout << servercontext->get_index() << std::endl;
        request.get_ClientSocket()->response = new Response( *((ServerContext*)servercontext) , request) ;
    }
    // only read the body if the method is not get or head! or if content length is not 0...
    if ( request.get_method() != "POST")
    {
        request.markBodyParsed(true);
        request.markAsReady(true);   
        return ;
    }
    if (!request.hasParsedBody())
        parse_body(request, rcvdMsg);
}

void    parse_client_request(Request & request, std::string & rcvdMsg)
{
    try
    {
        if (!request.hasParsedHeaders())
        {
            size_t null_term_pos = rcvdMsg.find('\0');
            for (size_t i = null_term_pos; i < rcvdMsg.length() ; ++i)
            {
                if ( rcvdMsg[i] != '\0' )
                    request.markAsBad(true);
            }
        }

        determine_parsing_stage(request, rcvdMsg);
    }
    catch(const char * e)
    {
        return ;
    }
}

void    handle_client_request(ClientSocket* client_info)
{
    std::string rcvdMsg;
    char        buffer[READ_BUFFER_SIZE];
    size_t      rcvdSize;

    if ((rcvdSize = recv(client_info->get_sock_fd(), (void *) buffer, READ_BUFFER_SIZE - 1, 0)) < 0)
        throw std::runtime_error(std::string("Webserv : recv() failed, reason : ") + strerror(errno));

    buffer[rcvdSize] = '\0';
    rcvdMsg.append(buffer, rcvdSize);
    
    // size_t crlf_pos;
    // crlf_pos = rcvdMsg.find(CRLF);
    // while (crlf_pos != std::string::npos)
    // {
    //     rcvdMsg.insert(crlf_pos, ".");
    //     crlf_pos = rcvdMsg.find(CRLF, crlf_pos + 3);
    // }
    // std::cerr << rcvdMsg << std::endl;
    // std::cout << "--------------" << std::endl;
    // return ;
    parse_client_request(*(client_info->request), rcvdMsg);
}

void    respond_to_client(ClientSocket* client_info, int kqueue_fd, int n_events, struct kevent * events)
{
    // (void)events;
    // (void)n_events;
    // if ( ! ((client_info->response.get _location && this->is_allowd_method_in_location() )|| (! this->_location && this->is_allowd_method())) )

    if ( ! client_info->request->isBadRequest() && client_info->response->is_cgi() && !client_info->response->p_is_running && client_info->response->get_exit_stat() == -1 )
    {
        struct kevent ev;
        CgiProcess * process = new CgiProcess();
        process->request = client_info->request;
        process->response = client_info->response;
        process->set_type('P');
        ft_memset(&ev, 0, sizeof(struct kevent));
        client_info->response->execute_cgi();
        EV_SET(&ev, client_info->response->get_process_id(), EVFILT_PROC, EV_ADD | EV_ENABLE , NOTE_EXIT, 0, (void *) process );
        if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
            throw std::runtime_error(std::string("Webserv : kevent(4) failed, reason : ") + strerror(errno));
        client_info->response->p_is_running = true ;
    }

    if ( ! client_info->request->isBadRequest() && client_info->response->is_cgi() && client_info->response->p_is_running && client_info->response->get_exit_stat() == -1)
    {
        for (int i = 0; i < n_events; i++)
        {
            if (((CgiProcess *) events[i].udata)->get_type() == 'P' && ((CgiProcess *) events[i].udata)->response == client_info->response && events[i].filter == EVFILT_PROC && events[i].fflags & NOTE_EXIT )
            {
                client_info->response->set_exit_stat(0);
                break ;
            }
        }
    }

    if ( !client_info->response->is_cgi() || ( client_info->response->is_cgi() && client_info->response->get_exit_stat() != -1 ))
    {
        std::string rsp;
        rsp = client_info->response->getResponse();
        if (send(client_info->get_sock_fd(), (void *) rsp.c_str(), rsp.length(), 0) == -1)
            throw std::runtime_error("send failed");
        if ( client_info->response->end_of_response() )
        {
            delete client_info->request;
            delete client_info->response;
            client_info->request = NULL;
            client_info->response = NULL;
            client_info->request = new Request();
            client_info->request->set_ClientSocket(client_info);
            switch_interest(client_info, kqueue_fd, EVFILT_WRITE, EVFILT_READ); // Interest is gonna be switched only if the response has been entirely sent.
        }
    }
}
