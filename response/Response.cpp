/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:37 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/03 13:05:28 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/main.hpp"

Response::Response ( ServerContext & server_context, Request & request) :\
                    server_context(server_context), request(request)
{
    this->_end_of_response = false ;
    this->_tranfer_encoding = false ;
    this->is_first_message = true ;
    this->_location         = NULL;
}
 
void    Response::format_message( void )
{
    short status = this->request.getStatus() ;

    // responde cleint errors (parse error ... )
    if ( status != -1)
    {
        this->error_response( status ) ;
    }
    // else if ( this->is_cgi_request() )
    // {
    //     // brahim object cgi
    // }
    else
    {
        try
        {
            this->get_static_page() ;
        }
        catch ( int error )
        {
            // this->_end_of_response = true ;
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

void    Response::error_response( short error )
{
    std::string err_page_path;

    err_page_path = this->find_error_page( error );

    std::cout << "the path of error page :  " << err_page_path << std::endl ;
    if (err_page_path == "" )
    {
        std::string  error_msg = default_error_pages.getErrorMsg( error ) ;
        
        std::string err_body = default_error_pages.getErrorPage( error_msg ) ;
        std::ostringstream  len;
        len << err_body.length() ;

        this->message.append( "HTTP/1.1 " + error_msg + "\r\nContent-Type: text/html\r\nContent-Length: " ) ;
        this->message.append(len.str() + "\r\nConnection: close\r\nServer: 1337WebServ\r\n\r\n") ;
        this->message.append(err_body) ;
        this->_end_of_response = true ;
    }
    else
    {
        this->_end_of_response = true ;
        std::cout << "the path of error page :  " << err_page_path << std::endl ;
    }
    
}

// POST /cgi-bin/script.cgi HTTP/1.1
bool    Response::is_cgi_request( void )
{
    std::string uri = this->request.get_request_target() ;
    // std::string uri = "/cgi-bin/script.cgi" ;

    if ( uri.length() > 4 && uri.substr( uri.length() - 4 , 4 ) == ".cgi")
    {
        return true ;
    }
    return false ;
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

/* Getters */

const std::string & Response::getResponse( void )
{
    this->message = "";
    this->format_message() ;
    return ( this->message ) ;
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
