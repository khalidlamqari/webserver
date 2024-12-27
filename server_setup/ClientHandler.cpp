/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 11:37:00 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/27 20:49:58 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


#include <signal.h>
#include "Socket.hpp"

void    determine_parsing_stage(ClientSocket* client_info, std::string & rcvdMsg)
{
    size_t      crlf_pos = rcvdMsg.find(CRLF);
    Request *   request = client_info->get_request();
    
    if ((!request->hasParsedStartLine() || !request->hasParsedHeaders()) && crlf_pos == std::string::npos)
        return request->storeUnparsedMsg(rcvdMsg); 

    rcvdMsg.insert(0, request->getUnparsedMsg());
    request->resetUnparsedMsg();
    crlf_pos = rcvdMsg.find(CRLF);

    if (!request->hasParsedStartLine())
    {
        parse_start_line(*(request), rcvdMsg.substr(0, crlf_pos));
        rcvdMsg.erase(0, crlf_pos + 2);
        return determine_parsing_stage(client_info, rcvdMsg);
    }
    
    if (!request->hasParsedHeaders())
    {
        parse_headers(*request, rcvdMsg);
        return determine_parsing_stage(client_info, rcvdMsg);
    }

    // ADD NEW
    if (!client_info->get_response())
    {
        client_info->set_response( new Response( *client_info) ); // TODO khalid : pass client info and remove request inside response
    }

    if (request->get_method() != "POST")
    {
        request->markBodyParsed(true);
        request->markAsReady(true);
        return; 
    }

    // only read the body if the method is not get or head! or if content length is not 0...
    if (!request->hasParsedBody())
    {
        parse_body(*request, rcvdMsg);
        if (!rcvdMsg.empty())
            request->storeUnparsedMsg(rcvdMsg); // no need ?
    }
}

void    parse_client_request(ClientSocket* client_info, std::string & rcvdMsg)
{
    try
    {
        // if (!request.hasParsedHeaders())
        // {
        //     size_t null_term_pos = rcvdMsg.find('\0');
        //     for (size_t i = null_term_pos; i < rcvdMsg.length() ; ++i)
        //     {
        //         if ( rcvdMsg[i] != '\0' )
        //             request.markAsBad(true);
        //     }
        // }

        determine_parsing_stage(client_info, rcvdMsg);
    }
    catch(const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        client_info->get_request()->markAsBad(3334);
        return ;
    }
}

void    handle_client_request(ClientSocket* client_info)
{
    std::string rcvdMsg = "";
    char        buffer[READ_BUFFER_SIZE];
    size_t      rcvdSize;

    if ((rcvdSize = recv(client_info->get_ident(), (void *) buffer, READ_BUFFER_SIZE - 1, 0)) < 0)
        throw std::runtime_error(std::string("Webserv : recv() failed, reason : ") + strerror(errno));

    buffer[rcvdSize] = '\0';
    rcvdMsg.append(buffer, rcvdSize);
    parse_client_request(client_info, rcvdMsg);
}

// ADD NEW

CgiProcess * create_cgi_process_ident(ClientSocket* client_info)
{
    CgiProcess * process = new CgiProcess();

    process->set_response(client_info->get_response());
    
    client_info->get_response()->execute_cgi();

    process->set_ident(client_info->get_response()->get_process_id());

    client_info->get_response()->set_cgi_process(process);
    
    return (process);
}



// ADD NEW

CgiPairSocket * create_pair_sock_ident(ClientSocket* client_info)
{
    CgiPairSocket * pair_soc = new CgiPairSocket();

    pair_soc->set_response(client_info->get_response());

    pair_soc->set_ident(client_info->get_response()->get_pair_fds()[0]);

    client_info->get_response()->set_cgi_pair_socket(pair_soc);

    return (pair_soc);
}

// ADD NEW


bool is_response_ready_to_sent(Response & response)
{
    int status = response.get_status();

    return (!response.is_cgi() || (status >= 400 && status <= 599) 
            || (response.is_cgi() && response.p_is_running
            && response.get_exit_stat() != -1));

}



/* create new request and delete old request and response or delete client  */
void create_new_request(ClientSocket* client_info, SocketManager& socketManager)
{
    // Response *response = client_info->get_response();
    
    std::cout << "end of response " << std::endl;
    if ( !client_info->get_request()->get_is_persistent())
    {
        socketManager.delete_client(client_info->get_ident());
    }
    else
    {
        client_info->delete_request();
        client_info->delete_response();
        client_info->set_request(new Request());
    }
}

static std::time_t max_time = 10; // sec

void check_timeout(Response & response) // TODO: 
{
    if (std::time(0) - response.get_start_time() > max_time)
    {
        kill(response.get_process_id(), SIGKILL);
        response.set_status(504);
    }
}

void send_response(int sock_fd, const std::string & msg)
{
    if (send(sock_fd, (void *) msg.c_str(), msg.length(), 0) == -1)
        throw std::runtime_error("send failed");
}

 
void    respond_to_client(ClientSocket* client_info, KqueueEventQueue & kqueueManager )
{
    Response *response = client_info->get_response();

    if ( response->is_cgi() && !response->p_is_running && response->get_exit_stat() == -1 ) /* check if process not started ( to avoid creation of multi process ) */
    {
        CgiProcess * process = create_cgi_process_ident(client_info);
        CgiPairSocket * pair_soc = create_pair_sock_ident(client_info);
        
        kqueueManager.register_event_in_kqueue(process, EVFILT_PROC);
        
        kqueueManager.register_event_in_kqueue(pair_soc, EVFILT_READ);
        
        client_info->get_response()->p_is_running = true ;
        response->set_start_time(std::time(0)); /* set start time  */
    }
    else if (is_response_ready_to_sent(*response))
    {
        std::string res = ""; // TODO : khalid
        try{
            res = response->getResponse();
        }
        catch(int x)
        {
            (void)x;
            return ;
        }
        send_response(client_info->get_ident(), res);
    }
    else if (response->is_cgi() && response->p_is_running && response->get_exit_stat() == -1) /* checking if process still running  */
    {
        check_timeout(*response);
    }

    // if ( is_response_finish(*response) )
    // {
    //     create_new_request(client_info, socketManager);
    // }
}
