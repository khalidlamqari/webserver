/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:37 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/07 13:19:46 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

// static size_t num_file = 0;

Response::Response ( ServerContext & server_context, Request & request) :\
                    server_context(server_context), request(request)
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
    this->is_parsed         = false;
    this->offset            = 0;

    // this->fd_input          = -1;
    process_target(this->request.get_request_target());
    if (!request.isBadRequest() && !this->_cgi_extention.empty() && (this->_path_.length() - this->_cgi_extention.length() > 0) && (this->_path_.find(this->_cgi_extention, this->_path_.length() - this->_cgi_extention.length()) != std::string::npos))
    {
        this->_is_cgi = true;
    }

    if ( this->_is_cgi )
    {
        if ( socketpair(AF_UNIX, SOCK_STREAM, 0, this->s_fds) == -1) // && close(this->s_fds[1]) == -1
            throw std::runtime_error("socketpair failed") ;
    }
}

void    Response::format_message( void )
{
    if ( this->request.isBadRequest() )
    {
        this->status = 500;
    }
    
    if (this->get_parse_stat() == 200)
    {
        try
        {
            this->get_static_page() ;
        }
        catch ( int error )
        {
            this->_tranfer_encoding = false ;
            this->is_first_message = true ;
            this->status = error ;
            this->error_response( error ) ;
        }
    }
    else
    {
        this->error_response( this->get_parse_stat() ) ;
    }
}

void    Response::error_response( short error )
{
    std::string err_page_path;

    if (this->_tranfer_encoding)
    {
        this->responde_with_overrided_page( error , this->_path_ );
        return ;
    }
    err_page_path = this->find_error_page( error );
    if ( err_page_path == "" )
    {
        this->responde_with_default_page( error );
    }
    else
    {
        this->responde_with_overrided_page( error , err_page_path ) ;
    }
}

void Response::responde_with_overrided_page( short error , std::string err_page_path )
{
    (void)error;
    if ( ! this->_tranfer_encoding )
    {
        this->_path_ = err_page_path ;
    }
    try
    {
        this->read_and_format_msg();
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
    this->message.append(err_body) ;
    this->_end_of_response = true ;
}

bool Response::is_allowd_method()
{
    /* search in location */
    if ( _location &&  (std::find( this->_location->get_allowed_methods().begin(), this->_location->get_allowed_methods().end(), \
    this->request.get_request_method() ) !=  this->_location->get_allowed_methods().end()) )
    {
        return ( true ) ;
    }
    /* search in server */
    if (!_location && (std::find(this->server_context.get_allowed_methods().begin(), this->server_context.get_allowed_methods().end(), \
    this->request.get_request_method()) != this->server_context.get_allowed_methods().end()) )
    {
        return ( true ) ;
    }
    return ( false ) ;
}

/* Getters */

const std::string & Response::getResponse( void )
{
    this->message = "";
    if ( this->_end_of_response )
        return ( this->message ) ;
    this->format_message() ;

    return ( this->message ) ;
}

std::string Response::find_error_page( unsigned short error )
{
    std::vector<std::pair <unsigned short, std::string> >::const_iterator i ;
    if ( this->_location )
    {
        const std::vector<std::pair <unsigned short, std::string> > & pages = this->_location->get_error_pages() ;
        for ( i = pages.begin() ; i != pages.end() ; ++i )
        {
            if ( i->first == error )
            {
                return ( this->_location->get_root_directory() + "/" + i->second ) ;
            }
        }
    }
    else
    {
        const std::vector<std::pair <unsigned short, std::string> > & pages = this->server_context.get_error_pages() ;
        for ( i = pages.begin() ; i != pages.end() ; ++i )
        {
            if ( i->first == error )
            {
                return ( this->server_context.get_root_directory() + "/" + i->second ) ; 
            }
        }
    }
    return ( "" ) ;
}

bool    Response::is_cgi() 
{
    return (this->_is_cgi) ;
}

bool Response::end_of_response()
{
    return ( this->_end_of_response ) ;
}

void Response::set_end_of_response(bool stat)
{
    this->_end_of_response = stat;
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
}

const std::string & Response::getUploadDir  ( void )
{
    return this->_upload_dir ;
}

int * Response::get_pair_fds()
{
    return ( this->s_fds );
}

const std::ifstream &         Response::getPageStream( void )
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

void    Response::set_exit_stat(int stat)
{
    this->exit_stat = stat;
}

LocationContext * Response::get_location()
{
    return this->_location;
}

int Response::get_parse_stat()
{
    return this->status;
}


const std::string & Response::get_connection() const
{
    return this->connection;
}

void Response::set_parse_stat(int stat)
{
    this->status = stat;
}