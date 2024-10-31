/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:37 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/31 16:11:28 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

ErrorPages default_error_pages ;

Response::Response ( ServerContext & server_context, Request & request) :\
                    server_context(server_context), request(request)
{}
 
void    Response::format_message( void )
{
    short status = this->request.getStatus() ;
    // short status = 505 ;
    std::cout << "am here 1" << std::endl;
    // responde cleint errors (parse error ... )
    // if ( (status >= 400 && status <= 418) || (status >= 421 && status <= 426) || status == 428 || status == 429 || status == 431 || status == 451 )
    if ( status != -1)
        this->error_response( status ) ;

    else if ( this->is_cgi_request() )
    {
        // brahim object cgi
    }
    else
    {
        try
        {
            this->get_static_page() ;
        }
        catch ( int error )
        {
            this->error_response( error ) ;
        }
        // this->message = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<h1 style='color:red;'>Hello from C++ server!</h1>" ;
    }

    // else statis file

}

/*
**    HTTP/1.1 400 Bad Request
**    Content-Type: text/html
**    Content-Length: 173
**    Connection: close
**    Server: nginx
*/

void    Response::error_response( short error )
{
    std::string  error_msg = default_error_pages.getErrorMsg( error ) ;
    
    std::string err_body = default_error_pages.getErrorPage( error_msg ) ;
    std::ostringstream  len;
    len << err_body.length() ;
    

    
    this->message.append( "HTTP/1.1 " + error_msg + "\r\n" ) ;
    this->message.append("Content-Type: text/html\r\n") ;
    this->message.append("Content-Length: " + len.str() + "\r\nConnection: close\r\n") ;
    this->message.append("Server: 1337WebServ\r\n\r\n") ;
    this->message.append(err_body) ;
    
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
    (void) server_context ;
    this->format_message() ;
    return ( this->message ) ;
    // return "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<h1 style='color:red;'>Hello from C++ server!</h1>" ;

}

Response::~Response() 
{
    
}
