/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:37 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/24 20:14:10 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"
// static size_t num_file = 0;
Response::Response ( ClientSocket & clientsocket ) : clientsocket(clientsocket)
{
    this->_end_of_response  = false;
    this->_tranfer_encoding = false;
    this->is_first_message  = true;
    this->_location         = NULL;
    this->exit_stat         = -1;
    this->status            = 200; 
    this->p_is_running      = false;
    this->_is_cgi           = false;
    this->s_fds[0]          = -1;
    this->s_fds[1]          = -1;
    this->pid               = -1;
    this->offset            = 0;
    this->_cgi_process      = NULL;
    this->_cgi_pair_socket  = NULL;
    this->server_context    = NULL;

    process_requset();
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
        this->responde_with_overrided_page( err_page_path ) ;
    }
}

 // TODO : khalid
void Response::responde_with_overrided_page( std::string err_page_path )
{
    if ( ! this->_tranfer_encoding )
    {
        this->_path_ = err_page_path;// TODO : khalid
        set_cgi_requerements(*this, _is_cgi, input_data); /* input_data is a ostream for pass body to child process (cgi) */
        if (_is_cgi)
            throw (int)10001;// TODO : khalid
    }
    try
    {
        // this->format_static_response();
        format_response();
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
    len << err_body.length() ;
    this->message.append( "HTTP/1.1 " + error_msg + "\r\nContent-Type: text/html\r\nContent-Length: " ) ;
    this->message.append(len.str() + "\r\nConnection: close\r\nServer: 1337WebServ\r\n\r\n") ;
    this->message.append(err_body);
    this->_end_of_response = true;
}

bool Response::is_allowed_method()
{
    /* search in location */
    if ( _location &&  (std::find( this->_location->get_allowed_methods().begin(), this->_location->get_allowed_methods().end(), \
    this->clientsocket.get_request()->get_method() ) !=  this->_location->get_allowed_methods().end()) )
    {
        return ( true ) ;
    }
    /* search in server */
    if (!_location && (std::find(this->server_context->get_allowed_methods().begin(), this->server_context->get_allowed_methods().end(), \
    this->clientsocket.get_request()->get_method()) != this->server_context->get_allowed_methods().end()) )
    {
        return ( true ) ;
    }
    return ( false ) ;
}

/* Getters */



std::string Response::find_error_page( unsigned short error )
{
    std::vector<t_error_page>::const_iterator i ;
    if ( this->_location )    /*    search in location context     */
    {
        const std::vector<t_error_page> & pages = this->_location->get_error_pages();
        for ( i = pages.begin() ; i != pages.end() ; ++i )
        {
            if ( i->err_code == error )
            {
                if (is_file(this->_location->get_root_directory() + "/" + i->path))
                    return ( this->_location->get_root_directory() + "/" + i->path ) ;
                else
                    return "";
            }
        }
    }
    else            /*       search in server context        */
    {
        const std::vector<t_error_page> & pages = this->server_context->get_error_pages();
        for ( i = pages.begin() ; i != pages.end() ; ++i )
        {
            if ( i->err_code == error )
            {
                if (is_file(this->server_context->get_root_directory() + "/" + i->path))
                    return ( this->server_context->get_root_directory() + "/" + i->path );
                else
                    return "";
            }
        }
    }
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
    if (s_fds[0] != -1)
        close(s_fds[0]);
        
    if (s_fds[1] != -1)
        close(s_fds[1]);
        
    if (this->input_path != "")
        remove(this->input_path.c_str()) ;
    
    if (_cgi_pair_socket)
    {
        delete _cgi_pair_socket;
        _cgi_pair_socket = NULL;
    }

    if (_cgi_process)
    {
        delete _cgi_process;
        _cgi_process = NULL;
    }
}

const std::string & Response::getUploadDir  ( void )
{
    return this->_upload_dir ;
}

int * Response::get_pair_fds()
{
    return ( this->s_fds );
}

const std::ifstream & Response::getPageStream( void )
{
    return ( this->page_content ) ;
}

pid_t   Response::get_process_id()
{
    return ( this->pid ) ;
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


std::time_t Response::get_start_time()
{
    return this->start_time;
}

/* setters */
const std::string & Response::get_input_path()
{
    return this->input_path;
}

void    Response::set_input_path(const std::string & path)
{
    this->input_path = path;
}
    
void    Response::set_data_to_input(const std::string & data)
{
    this->input_data << data;
}

void Response::set_status(int stat)
{
    this->status = stat;
}

void Response::set_start_time(std::time_t tm)
{
    this->start_time = tm;
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