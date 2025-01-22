

#include "KqueueIdent.hpp"

/* --------------- Cgi Process -------------- */

CgiProcess::CgiProcess(Response *response): response(response), is_running(false)
{
    execute_cgi();
    this->_type = CHILD_ID;
    is_running = true;
}

CgiProcess::~CgiProcess()
{
    if (is_running && _ident)
    {
        kill(_ident, SIGKILL);
        waitpid(_ident, NULL, 0);
    }
}

void			CgiProcess::set_ident( const int & id )
{
    this->_ident = id;
}

void			CgiProcess::set_type( const t_ident_type & type )
{
    this->_type = type;
}

void			CgiProcess::set_response( Response * response )
{
    this->response = response;
}

int				CgiProcess::get_ident( void )
{
    return (this->_ident);
}

t_ident_type	CgiProcess::get_type( void )
{
    return ( this->_type );
}

Response*		CgiProcess::get_response( void )
{
    return (this->response);
}

void CgiProcess::setup_environment()
{
    setenv("REQUEST_METHOD", response->client_info.get_request()->get_method().c_str(), 1);
    setenv("QUERY_STRING",   response->client_info.get_request()->get_query().c_str(), 1);
    setenv("PATH_INFO", response->get_path_info().c_str(), 1);
    setenv("SCRIPT_FILENAME", response->get_path().c_str(), 1);
    setenv("REDIRECT_STATUS", std::stringstream(response->get_status()).str().c_str(), 1);
    setenv("SCRIPT_NAME", response->get_target().c_str() , 1);
    if (response->client_info.get_request()->get_method() == "POST")
    {
        setenv("CONTENT_LENGTH", std::stringstream (response->client_info.get_request()->get_content_length()).str().c_str(), 1);
    }

    std::map<std::string, std::string> headers = response->client_info.get_request()->get_headers();
    std::map< std::string , std::string>::iterator it;

    for (it = headers.begin(); it != headers.end(); ++it)
    {
        setenv(it->first.c_str(), it->second.c_str(), 1);
    }
}

void			CgiProcess::execute_cgi()
{
    _ident = fork();
    
    if (_ident == -1)
    {
        std::cerr << "EOFORK" << std::endl;
        // exit(0);
        throw std::runtime_error("fork failed");
    }
    if (_ident == 0)
    {
        char *av[3];
        response->get_exec_path(av);
        setup_environment();
        if (close(response->get_cgi_pair_socket()->get_pair_fds()[0])  == -1)
            exit(99);

        int fd = open(response->get_input_path().c_str(), O_RDONLY , 0777);
        if (fd != -1 && (dup2(fd, 0) == -1 || close(fd) == -1))
            exit(88);

        if (dup2(response->get_cgi_pair_socket()->get_pair_fds()[1], 1) == -1)
            exit(77);

        if (close(response->get_cgi_pair_socket()->get_pair_fds()[1])  == -1)
            exit(66);

        execve(av[0], av, environ);
        exit(11);
    }
}

void    CgiProcess::markAsStoped()
{
    is_running = false;
}

std::time_t CgiProcess::get_start_time()
{
    return this->start_time;
}

void CgiProcess::set_start_time(std::time_t tm)
{
    this->start_time = tm;
}

/* --------------- Cgi Pair Socket --------------- */

CgiPairSocket::CgiPairSocket(Response *response) : response(response)
{
    s_fds[0] = -1;
    s_fds[1] = -1;
    create_socket_pair();
    this->_ident = s_fds[0];
    this->_type = CGI_PAIR_SOCK;
}

CgiPairSocket::~CgiPairSocket()
{
    if (s_fds[0] != -1)
        close(s_fds[0]);
        
    if (s_fds[1] != -1)
        close(s_fds[1]);
}

void			CgiPairSocket::set_ident( const int & id )
{
    this->_ident = id;
}

void			CgiPairSocket::set_type( const t_ident_type & type )
{
    this->_type = type;
}

void			CgiPairSocket::set_response( Response * response )
{
    this->response = response;
}

int				CgiPairSocket::get_ident( void )
{
    return ( this->_ident );
}

t_ident_type	CgiPairSocket::get_type( void )
{
    return ( this->_type );
}

Response*		CgiPairSocket::get_response( void )
{
    return (this->response);
}

void   CgiPairSocket::create_socket_pair( void )
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, s_fds ) == -1)
        throw std::runtime_error("Socket pair failed!");
}

int * CgiPairSocket::get_pair_fds()
{
    return ( this->s_fds );
}