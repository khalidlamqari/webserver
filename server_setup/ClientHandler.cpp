
#include "Server.hpp"
#include <signal.h>
#include <algorithm>

static void performPrimaryCheck(ClientSocket* client_info)
{
	if (client_info->get_request()->get_method() == "POST"
        && ! client_info->get_request()->isChunked()
        && !client_info->get_request()->ContentLengthIsSet())
    {       
		client_info->get_request()->markAsBad(400, __FILE__, __LINE__);
    }

    if (client_info->get_request()->get_version() == "HTTP/1.1" && !client_info->get_request()->hostIsSet())
		client_info->get_request()->markAsBad(400, __FILE__, __LINE__);

    client_info->get_response()->process_request();
    
    /*  check for client error or server error */
    if (client_info->get_response()->get_status() >= 400 && client_info->get_response()->get_status() <= 599)
        client_info->get_request()->markAsBad(client_info->get_response()->get_status(), __FILE__, __LINE__);
}

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
        if (request->hasParsedHeaders())
            performPrimaryCheck(client_info);

        return determine_parsing_stage(client_info, rcvdMsg);
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
        determine_parsing_stage(client_info, rcvdMsg);
    }
    catch(const char * e)
	{
        client_info->get_response()->process_request();
        client_info->get_response()->set_status(client_info->get_request()->bad_status_code);
		return ;
	}
}

void    handle_client_request(ClientSocket* client_info)
{
    std::string rcvdMsg = "";
    char        buffer[READ_BUFFER_SIZE];
    size_t      rcvdSize;

    if ((rcvdSize = recv(client_info->get_ident(), (void *) buffer, READ_BUFFER_SIZE - 1, 0)) < 0)
        return ;

    buffer[rcvdSize] = '\0';
    rcvdMsg.append(buffer, rcvdSize);
    parse_client_request(client_info, rcvdMsg);
}

// ADD NEW

CgiProcess * create_cgi_process_ident(ClientSocket* client_info)
{
    if (!client_info->get_response()->get_cgi_pair_socket())
        return NULL;

    CgiProcess * process = NULL;

    try
    {
        process = new CgiProcess(client_info->get_response());
    }
    catch(const std::exception& e)
    {
        delete client_info->get_response()->get_cgi_pair_socket();
        client_info->get_response()->set_cgi_pair_socket(NULL);
        delete process;
        return NULL;
    }

    client_info->get_response()->set_cgi_process(process);

    return (process);
}

// ADD NEW

CgiPairSocket * create_pair_sock_ident(ClientSocket* client_info)
{
    CgiPairSocket * pair_soc = NULL;
    try
    {
        pair_soc = new CgiPairSocket(client_info->get_response());
    }
    catch ( const std::exception & e)
    {
        delete pair_soc;
        return NULL;
    }

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
void setup_request_response(ClientSocket* client_info, SocketManager& socketManager)
{
    client_info->delete_request();
    client_info->delete_response();
    
    try
    {
        client_info->set_request(new Request());
        client_info->set_response(new Response(*client_info));
    }
    catch(const std::exception& e)
    {
        socketManager.delete_client(client_info->get_ident());
    }
}

bool    checkBrokenIdents(int ident, std::string cmd)
{
    static std::vector<int> broken_idents;
    
    if (cmd == "ADD")
        broken_idents.push_back(ident);
    else if (cmd == "CHECK")
    {
        std::vector<int>::iterator it = find(broken_idents.begin(), broken_idents.end(), ident);
        if (it != broken_idents.end())
        {
            broken_idents.erase(it);
            return true;
        }
    }
    else if (cmd == "CLEAR")
        broken_idents.clear();
    return false;
}

void check_timeout(Response & response, KqueueEventQueue & kqueueManager)
{
    if (response.server_context->get_cgi_read_time_out() == 0)
        return ;

    if (std::time(0) - response.get_cgi_process()->get_start_time() > response.server_context->get_cgi_read_time_out())
    {
        response.set_status(504);
        kqueueManager.delete_event_from_kqueue(response.get_cgi_process(), EVFILT_PROC);
        kqueueManager.delete_event_from_kqueue(response.get_cgi_pair_socket(), EVFILT_READ);
        checkBrokenIdents(response.get_cgi_process()->get_ident(), "ADD");
        checkBrokenIdents(response.get_cgi_pair_socket()->get_ident(), "ADD");
        delete response.get_cgi_process();
        response.set_cgi_process(NULL);
        delete response.get_cgi_pair_socket();
        response.set_cgi_pair_socket(NULL);
        response.set_is_cgi(false);
        global_status(504, true);
    }
}

void send_response(int sock_fd, const std::string & msg)
{
    if (send(sock_fd, (void *) msg.c_str(), msg.length(), 0) == -1)
    {
        throw std::runtime_error("send failed");
    }
}

void    setup_cgi(ClientSocket* client_info, KqueueEventQueue & kqueueManager)
{
    Response *response = client_info->get_response();

    CgiPairSocket * pair_soc = create_pair_sock_ident(client_info);
    CgiProcess * process = create_cgi_process_ident(client_info);

    if (!process)
    {
        response->set_is_cgi(false);
        response->set_status(500);
        return ;
    }

    if (kqueueManager.register_event_in_kqueue(process, EVFILT_PROC) 
        || kqueueManager.register_event_in_kqueue(pair_soc, EVFILT_READ))
    {
        // Deleting the previously registred events in the event queue.
        kqueueManager.delete_event_from_kqueue(process, EVFILT_PROC);

        delete pair_soc;
        delete process;
        response->set_cgi_pair_socket(NULL);
        response->set_cgi_process(NULL);
    
        // Set the request as non-CGI and indicate an internal server error (500)
        response->set_is_cgi(false);
        response->set_status(500);
    
        return ;
    }

    client_info->get_response()->p_is_running = true ;
    response->get_cgi_process()->set_start_time(std::time(0)); /* set start time  */
}

void    respond_to_client(ClientSocket* client_info, KqueueEventQueue & kqueueManager)
{
    Response *response = client_info->get_response();

     /* check if process not started ( to avoid creation of multi process ) */
    if ( response->is_cgi() && !response->p_is_running && response->get_exit_stat() == -1 && client_info->get_request()->isReady() )
    {
        setup_cgi(client_info, kqueueManager);
    }
    else if (is_response_ready_to_sent(*response))
    {
        send_response(client_info->get_ident(), response->getResponse());
    }
    else if (response->is_cgi() && response->p_is_running && response->get_exit_stat() == -1) /* checking if process still running  */
    {
        check_timeout(*response, kqueueManager);
    }
}
