/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:37 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/12 00:23:09 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

Response::Response ( ServerContext & server_context, Request & request) :\
                    server_context(server_context), request(request)
{
    this->_end_of_response = false ;
    this->_tranfer_encoding = false ;
    this->is_first_message = true ;
    this->_location         = NULL ;
    this->_running_post     = false ;
    // this->status            = 200 ;
    this->status = this->request.getStatus() ;
}
 
void    Response::format_message( void )
{
    // this->status = this->request.getStatus() ;

    // responde cleint errors (parse error ... )
    if ( this->status != 200 )
    {
        this->error_response( this->status ) ;
    }
    else
    {
        try
        {
            this->get_static_page() ;
        }
        catch ( int error )
        {
            // this->_end_of_response = true ;
            this->_tranfer_encoding = false ;
            this->is_first_message = true ;
            this->status = error ;
            this->error_response( error ) ;
        }
    }
}

/*
**    HTTP/1.1 400 Bad Request
**    Content-Type: text/html
**    Content-Length: 173
**    Connection: close
**    Server: nginx
*/

const std::ifstream &         Response::getPageStream( void )
{
    return ( this->page_content ) ;
}

void    Response::error_response( short error )
{
    std::string err_page_path ;

    if ( this->_tranfer_encoding )
    {
        this->responde_with_overrided_page( error , "" ) ; // if transfer already start
        return ;
    }
    err_page_path = this->find_error_page( error ) ;
    if ( err_page_path == "" )
    {
        this->responde_with_default_page( error );
    }
    else
    {
        this->responde_with_overrided_page( error , err_page_path ) ;
        // this->_end_of_response = true ;
    }
}

void Response::responde_with_overrided_page( short error , std::string err_page_path )
{
    if ( ! this->_tranfer_encoding )
    {
        this->page_content.open( err_page_path ) ;
        if ( ! this->page_content.is_open() )
        {
            this->responde_with_default_page( error ) ;
            return ;
        }
    }

    char    buffer[ RESP_BUFF ] = {0} ;
    this->page_content.read( buffer, RESP_BUFF - 1 ) ; // TODO: should check if reading is done or not
    if ( this->page_content.fail() && ! this->page_content.eof() )
    {
        this->responde_with_default_page( 500 ) ;
        return ;
    }
    buffer[this->page_content.gcount()] = '\0' ;
    if ( this->page_content.eof())
        this->_end_of_response = true ;
    else
        this->_tranfer_encoding = true ;
    this->generate_message(buffer, this->page_content.gcount() ) ;
}

void Response::responde_with_default_page( short error )
{
    std::string         error_msg      = default_error_pages.getErrorMsg( error ) ;
    std::string         err_body       = default_error_pages.getErrorPage( error_msg ) ;
    std::ostringstream  len;

    len << err_body.length() ;
    this->message.append( "HTTP/1.1 " + error_msg + "\r\nContent-Type: text/html\r\nContent-Length: " ) ;
    this->message.append(len.str() + "\r\nConnection: close\r\nServer: 1337WebServ\r\n\r\n") ;
    this->message.append(err_body) ;
    this->_end_of_response = true ;
}

// POST /cgi-bin/script.cgi HTTP/1.1
bool    Response::is_cgi( const std::string & path , const std::string & cgi_extention )
{
    if ( cgi_extention.length() > path.length() || cgi_extention.empty() )
        return ( false ) ;

    size_t j = path.length() - 1 ;
    for ( ssize_t i = cgi_extention.length() - 1 ; i >= 0 ; i-- )
    {
        if ( cgi_extention[i] != path[j--] )
            return ( false ) ;
    }
    return ( true ) ;
}

bool Response::is_allowd_method()
{
    if ( std::find( this->server_context.get_allowed_methods().begin(), this->server_context.get_allowed_methods().end(), \
    this->request.get_request_method() ) !=  this->server_context.get_allowed_methods().end() )
    {
        return ( true ) ;
    }
    return ( false ) ;
}

bool Response::is_allowd_method_in_location()
{
    if ( std::find( this->_location->get_allowed_methods().begin(), this->_location->get_allowed_methods().end(), \
    this->request.get_request_method() ) !=  this->_location->get_allowed_methods().end() )
    {
        return ( true ) ;
    }
    return ( false ) ;
}

/* Getters */

const std::string & Response::getResponse( void )
{
    this->message = "";
    this->format_message() ;
    
    return ( this->message ) ;
}

std::string Response::find_error_page( unsigned short error )
{
    std::vector<std::pair <unsigned short, std::string> >::iterator i ;
    if ( this->_location )
    {
        std::vector<std::pair <unsigned short, std::string> > & pages = this->_location->get_error_pages() ;
        for ( i = pages.begin() ; i != pages.end() ; ++i )
        {
            if ( i->first == error )
            {
                return ( this->_location->get_root_directory() + "/" + i->second ) ;
            }
        }
    }

    std::vector<std::pair <unsigned short, std::string> > & pages = this->server_context.get_error_pages() ;
    for ( i = pages.begin() ; i != pages.end() ; ++i )
    {
        if ( i->first == error )
        {
            return ( this->server_context.get_root_directory() + "/" + i->second ) ; 
        }
    }
    
    return ( "" ) ;
}

void    Response::upload_data(const std::string & file_name, const std::string & data )
{
    
    if ( this->files_to_upload.find(file_name) == this->files_to_upload.end() )
    {
        this->files_to_upload[file_name].open(this->server_context.get_upload_dir() + "/" + file_name);
    }
    this->files_to_upload[file_name] << data ;
}

bool Response::end_of_response()
{
    return ( this->_end_of_response ) ;
}

bool Response::tranfer_encoding()
{
    return ( this->_tranfer_encoding );
}

Response::~Response() 
{
}
