
# include "Response.hpp"
# include "DefaultInfo.hpp"
#include <signal.h>

Response::Response ( ClientSocket & client_info ) : client_info(client_info)
{
    _end_of_response  = false;
    _tranfer_encoding = false;
    is_first_message  = true;
    _location         = NULL;
    exit_stat         = -1;
    status            = 200;
    p_is_running      = false;
    _is_cgi           = false;
    offset            = 0;
    _cgi_process      = NULL;
    _cgi_pair_socket  = NULL;
    server_context    = NULL;
    is_processed       = false;
    body_size          = 0;
}

const std::string & Response::getResponse( void )
{
    message = "";
    if ( _end_of_response )
        return ( message );

    /*  checking if an error in request ( request parse ) */
    if ( get_status() >= 400 && get_status() <= 599)
    {
        error_response(get_status());
        return message;
    }

    try
    {
        format_response();
    }
    catch(int status_code)
    {
        message = "";
        _tranfer_encoding = false;
        is_first_message = true;
        status = status_code;
        _is_cgi = false;
        error_response(status_code);
    }
    return message;
}

void    Response::error_response( short error )
{
    std::string err_page_path;

    if (this->_tranfer_encoding)
    {
        this->responde_with_overrided_page( this->_path_ );
        return ;
    }

    err_page_path = this->find_error_page( error );
    if ( err_page_path == "" )
    {
        this->responde_with_default_page( error );
    }
    else
    {
        this->responde_with_overrided_page( err_page_path );
    }
}

void Response::responde_with_overrided_page( std::string err_page_path )
{
    if (! this->_tranfer_encoding )
        _path_ = err_page_path;

    try
    {
        format_static_response();
    }
    catch(int err)
    {
        this->responde_with_default_page( err );
    }
}

void Response::responde_with_default_page( short error )
{
    std::string         error_msg = default_info.getCodeMsg( error ) ;
    std::string         err_body = default_info.getDefaultPage( error_msg ) ;
    
    std::ostringstream  len;
    
    body_size = err_body.length();
    len << err_body.length() ;
    this->message.append( "HTTP/1.1 " + error_msg + "\r\nContent-Type: text/html\r\nContent-Length: " ) ;
    this->message.append(len.str() + "\r\nConnection: close\r\nServer: 1337WebServ\r\n\r\n") ;
    this->message.append(err_body);
    client_info.get_request()->markAsPersistent(false);
    this->_end_of_response = true;
}

bool Response::is_allowed_method()
{
    /* search in location */
    if ( _location &&  (std::find( this->_location->get_allowed_methods().begin(), this->_location->get_allowed_methods().end(), \
    this->client_info.get_request()->get_method() ) !=  this->_location->get_allowed_methods().end()) )
    {
        return ( true ) ;
    }
    /* search in server */
    if (!_location && (std::find(this->server_context->get_allowed_methods().begin(), this->server_context->get_allowed_methods().end(), \
    this->client_info.get_request()->get_method()) != this->server_context->get_allowed_methods().end()) )
    {
        return ( true ) ;
    }
    return ( false ) ;
}

/* Getters */


std::string get_page_from_location(LocationContext * location, unsigned short error)
{
    std::vector<t_error_page>::const_iterator i ;

    const std::vector<t_error_page> pages = location->get_error_pages();
    for ( i = pages.begin() ; i != pages.end() ; ++i )
    {
        if ( i->err_code == error )
        {
            if (is_file(location->get_root_directory() + "/" + i->path))
                return ( location->get_root_directory() + "/" + i->path );
            else
                return "";
        }
    }
    return "";
}

std::string get_page_from_server(const ServerContext * server_context, unsigned short error)
{
    std::vector<t_error_page>::const_iterator i ;
    const std::vector<t_error_page> pages = server_context->get_error_pages();

    for ( i = pages.begin() ; i != pages.end() ; ++i )
    {
        if ( i->err_code == error )
        {
            if (is_file(server_context->get_root_directory() + "/" + i->path))
                return ( server_context->get_root_directory() + "/" + i->path );
            else
                return "";
        }
    }
    return "";
}

std::string Response::find_error_page( unsigned short error )
{

    /* search in location context */
    if ( this->_location )    
        return get_page_from_location(_location, error);


     /* search in server context */
    else
        return get_page_from_server(server_context, error);


    return ( "" ) ;
}

bool    Response::is_cgi()
{
    return (this->_is_cgi);
}

bool Response::end_of_response()
{
    return ( this->_end_of_response );
}

bool Response::tranfer_encoding()
{
    return ( this->_tranfer_encoding );
}

Response::~Response()
{

        
    if (this->input_path != "")
        remove(this->input_path.c_str());

    if (_cgi_pair_socket)
    {
        checkBrokenIdents(_cgi_pair_socket->get_ident(), "ADD");
        delete _cgi_pair_socket;
        _cgi_pair_socket = NULL;
    }
    if (_cgi_process)
    {
        checkBrokenIdents(_cgi_process->get_ident(), "ADD");
        delete _cgi_process;
        _cgi_process = NULL;
    }
    
}

const std::string & Response::getUploadDir  ( void )
{
    return this->_upload_dir ;
}



const std::ifstream & Response::getPageStream( void )
{
    return ( this->page_content ) ;
}

int   Response::get_exit_stat()
{
    return ( this->exit_stat ) ;
}

LocationContext * Response::get_location()
{
    return this->_location;
}

int Response::get_status()
{
    return this->status;
}

const std::string & Response::get_input_path()
{
    return this->input_path;
}

void    Response::set_input_path(const std::string & path)
{
    this->input_path = path;
}

void Response::set_status(int stat)
{
    this->status = stat;
}

void    Response::set_exit_stat(int stat)
{
    this->exit_stat = stat;
}

void Response::set_end_of_response(bool stat)
{
    this->_end_of_response = stat;
}

CgiProcess*		Response::get_cgi_process(void)
{
    return (this->_cgi_process);
}

CgiPairSocket* 	Response::get_cgi_pair_socket(void)
{
    return (this->_cgi_pair_socket);    
}

void            Response::set_cgi_process(CgiProcess* proc)
{
    this->_cgi_process = proc;
}

void    Response::set_cgi_pair_socket(CgiPairSocket* cgi_sock)
{
    this->_cgi_pair_socket = cgi_sock;    
}

void    Response::set_path(const std::string & path)
{
    this->_path_ = path;
}

void    Response::set_cgi_extention(const std::string & ext)
{
    this->_cgi_extention = ext;
}

void Response::set_exec_path(const std::string & exec)
{
    this->exec_path = exec;
}

void    Response::set_upload_dir(const std::string & dir)
{
    this->_upload_dir = dir;
}

void    Response::set_target(const std::string & target)
{
    this->_target = target;
}


const std::string &         Response::get_target(void) const
{
    return (this->_target);
}

const std::string &         Response::get_path(void) const
{
    return (this->_path_);
}

const std::string &        Response::get_cgi_exrention(void) const
{
    return (this->_cgi_extention);
}

const std::string & Response::get_upload_dir() const
{
    return (this->_upload_dir);
}


void         Response::get_exec_path( char * av[])
{
    if (exec_path == "")
    {
        av[0] = (char *)_path_.c_str();
        av[1] = NULL;
    }
    else
    {
        av[0] = (char *)exec_path.c_str();
        av[1] = (char *)_path_.c_str();
        av[2] = NULL;
    }
    
}

const std::string & Response::get_path_info() const
{
    return (this->_path_info);
}



void    Response::set_is_cgi(bool is_cgi)
{
    this->_is_cgi = is_cgi;
}

std::string &               Response::get_data_out(void)
{
    return this->data_out;
}