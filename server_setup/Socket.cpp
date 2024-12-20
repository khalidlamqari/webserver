


#include "Socket.hpp"

/* Socket methods definition */

Socket::Socket()
{
    this->_ident_is_set = false; // because if we dont, creating a socket without setting it s ident will result in closing a random fd after destructing that object.
}

Socket::~Socket()
{
    if(this->_ident_is_set && this->_ident >= 0)
    {
        close(this->_ident);
    }
}

/* ListenerSocket methods definition */

ListenerSocket::ListenerSocket()
{
    this->set_type(LISTENER_SOCK);
    this->is_temp = false;
}

ListenerSocket::ListenerSocket( const bool & is_temp ) : is_temp(is_temp)
{
    
}

ListenerSocket::~ListenerSocket()
{

}

void    ListenerSocket::mark_ident_as_set()
{
    this->_ident_is_set = true;
}

void    ListenerSocket::set_ident( const int & id )
{
    this->_ident = id;
}

void    ListenerSocket::set_type( const t_ident_type & type )
{
    this->_type = type;
}

void    ListenerSocket::set_host( const struct in_addr & host )
{
    this->_host = host;
}

void    ListenerSocket::set_port( const unsigned short & port )
{
    this->_port = port;
}

void    ListenerSocket::set_servers( const std::vector<const ServerContext*> & servers )
{
    this->_related_servers = servers;
}

void    ListenerSocket::add_server( const ServerContext* server )
{
    this->_related_servers.push_back(server);
}

int   ListenerSocket::get_ident( void )
{
    return this->_ident;
}

t_ident_type    ListenerSocket::get_type( void )
{
    return this->_type;
}

struct in_addr  ListenerSocket::get_host( void )
{
    return this->_host;
}

unsigned short  ListenerSocket::get_port( void )
{
    return this->_port;
}

const std::vector<const ServerContext*>&	ListenerSocket::get_servers( void )
{
    return this->_related_servers;
}

ListenerSocket& ListenerSocket::get_instance( void )
{
    return *this;
}

/* ClientSocket methods definition */

ClientSocket::ClientSocket() : _request(NULL)
{
    this->set_type(CLIENT_SOCK);
    // ADD NEW
    this->_response = NULL;
}

ClientSocket::~ClientSocket()
{
    if (this->_request)
        delete this->_request;
}

void    ClientSocket::set_ident( const int & id )
{
    this->_ident_is_set = true;
    this->_ident = id;
}

void    ClientSocket::set_type( const t_ident_type & type )
{
    this->_type = type;
}

void    ClientSocket::set_host( const struct in_addr & host )
{
    this->_host = host;
}

void    ClientSocket::set_port( const unsigned short & port )
{
    this->_port = port;
}

void    ClientSocket::set_servers( const std::vector<const ServerContext*> & servers )
{
    this->_related_servers = servers;
}

void    ClientSocket::add_server( const ServerContext* server )
{
    this->_related_servers.push_back(server);
}

int   ClientSocket::get_ident( void )
{
    return this->_ident;
}

t_ident_type    ClientSocket::get_type( void )
{
    return this->_type;
}

struct in_addr  ClientSocket::get_host( void )
{
    return this->_host;
}

unsigned short  ClientSocket::get_port( void )
{
    return this->_port;
}

const std::vector<const ServerContext*>&	ClientSocket::get_servers( void )
{
    return this->_related_servers;
}

ClientSocket& ClientSocket::get_instance( void )
{
    return *this;
}

void    ClientSocket::set_request( Request *new_request )
{
    this->_request = new_request;
}

Request*	ClientSocket::get_request( void )
{
    return this->_request;
}

void    ClientSocket::delete_request( void )
{
    if (this->_request)
        delete this->_request;

    this->_request = NULL;
}

void    ClientSocket::set_response( Response *new_response )
{
    this->_response = new_response;
}

Response*   ClientSocket::get_response( void )
{
    return ( this->_response );
}

void    ClientSocket::delete_response( void )
{
    if (this->_response)
        delete this->_response;

    this->_response = NULL;
}

/* --------------- Cgi Process -------------- */

CgiProcess::CgiProcess()
{
    this->_ident = -1;
    this->_type = CHILD_ID;
    this->response = NULL;
}

CgiProcess::CgiProcess(const int & id)
{
    this->_type = CHILD_ID;
    this->_ident = id;
    this->response = NULL;
}

CgiProcess::~CgiProcess()
{
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


/* --------------- Cgi Pair Socket --------------- */

CgiPairSocket::CgiPairSocket()
{
    this->_ident = -1;
    this->_type = CGI_PAIR_SOCK;
    this->response = NULL;
}

CgiPairSocket::CgiPairSocket(const int & id)
{
    this->_type = CGI_PAIR_SOCK;
    this->_ident = id;
    this->response = NULL;
}

CgiPairSocket::~CgiPairSocket()
{
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
