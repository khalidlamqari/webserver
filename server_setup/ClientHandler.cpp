/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 11:37:00 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/19 19:08:39 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <signal.h>

static std::time_t max_time = 10; // sec

void delete_client(std::vector<ClientSocket *>& activeClients, int fd)
{
    std::vector<ClientSocket *>::iterator it = activeClients.begin();
    std::vector<ClientSocket *>::iterator end = activeClients.end();

    for ( ; it != end; it++)
    {
        if ((*it)->get_sock_fd() == fd) // TODO : why dont i store clients in a map instead of a vector like that the key will be the client fd so i dont have to loop over all clients to delete on of them
        {
            if ( (*it)->response && (*it)->response->get_process_id() != -1 && (*it)->response->get_process_id() != 0 )
            {
                kill((*it)->response->get_process_id(), SIGKILL);
            }
            if (((*it)->cgiprocess && (*it)->cgiprocess->response->get_exit_stat() != -1) || !(*it)->cgiprocess)
            {
                if ((*it)->cgiinfo)
                {
                    (*it)->cgiinfo->request = NULL;
                    (*it)->cgiinfo->response = NULL;
                    delete (*it)->cgiinfo;
                    (*it)->cgiinfo  = NULL;
                }
                if ((*it)->cgiprocess )
                {
                    (*it)->cgiprocess->request = NULL;
                    (*it)->cgiprocess->response = NULL;
                    delete (*it)->cgiprocess;
                    (*it)->cgiprocess  = NULL;
                }
                delete (*it)->request;
                delete (*it)->response;
                (*it)->request = NULL;
                (*it)->response  = NULL;
                delete (*it);
                activeClients.erase(it);
            }
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
    new_client->response = NULL;
    new_client->cgiinfo = NULL;
    new_client->cgiprocess = NULL;

    new_client->request->set_ClientSocket(new_client);// should change this

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
        request.get_ClientSocket()->response = new Response( *((ServerContext*)servercontext), request );
    }

    // only read the body if the method is not get or head! or if content length is not 0 ...
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
    parse_client_request(*(client_info->request), rcvdMsg);
}

void    register_process_to_kqueue(ClientSocket* client_info, int kqueue_fd)
{
    struct kevent ev;
    CgiProcess * proc = new CgiProcess();

    proc->response = client_info->response;
    proc->set_type('P');
    client_info->cgiprocess = proc;
    client_info->response->execute_cgi();
    ft_memset(&ev, 0, sizeof(struct kevent));
    EV_SET(&ev, client_info->response->get_process_id(), EVFILT_PROC, EV_ADD | EV_ENABLE , NOTE_EXITSTATUS , 0, (void *)proc );
    kevent(kqueue_fd, &ev, 1, NULL, 0, NULL);
    if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
    {
        throw std::runtime_error(std::string("Webserv12 : kevent(4) failed, reason : ") + strerror(errno));
    }
    client_info->response->p_is_running = true ;
}

void    register_sock_pair_to_kqueue(ClientSocket* client_info, int kqueue_fd)
{
    struct kevent ev;
    CgiInfo * fd_pair_soc = new CgiInfo();

    fd_pair_soc->set_type('G');
    fd_pair_soc->response = client_info->response;
    client_info->cgiinfo = fd_pair_soc;
    fcntl(fd_pair_soc->response->get_pair_fds()[0], F_SETFL, O_NONBLOCK);
    ft_memset(&ev, 0, sizeof(struct kevent));
    EV_SET(&ev, fd_pair_soc->response->get_pair_fds()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, (void *)fd_pair_soc);
    if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
    {
        throw std::runtime_error(std::string(("Webserv2 : kevent(4) failed, reason : ")) + strerror(errno));
    }
}

void    respond_to_client(ClientSocket* client_info, int kqueue_fd)
{
    Response *response = client_info->response;
    int status = response->get_stat();

    if ( response->is_cgi() && !response->p_is_running && response->get_exit_stat() == -1 )
    {
        register_process_to_kqueue(client_info, kqueue_fd);
        register_sock_pair_to_kqueue(client_info, kqueue_fd);

        /* set start time  */
        response->set_start_time(std::time(0)); // current time in seconds
        return;
    }

    if (!response->is_cgi() || (status >= 400 && status <= 599) || (response->is_cgi() && response->p_is_running && response->get_exit_stat() != -1))
    {
        std::string rsp = response->getResponse();
        if (send(client_info->get_sock_fd(), (void *) rsp.c_str(), rsp.length(), 0) == -1)
            throw std::runtime_error("send failed");
    }
    else if (response->is_cgi() && response->p_is_running && response->get_exit_stat() == -1)
    {
        /* check timeout */
        if (std::time(0) - response->get_start_time() > max_time)
        {
            kill(response->get_process_id(), SIGKILL);
            response->set_stat(504);
        }
    }
}
