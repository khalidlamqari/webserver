/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:37 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/24 19:18:40 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response ( ServerContext & server_context, Request & request, int error_number) :\
                    server_context(server_context), request(request), error_number(error_number)
{
}

void    Response::format_message( void )
{
    std::cout << "error number : " << this->error_number << std::endl;
    std::vector<std::string> & allowed_methods = this->server_context.get_allowed_methods() ;
    const std::vector<std::string> & server_names = this->server_context.get_server_names() ;
    std::map< std::string , std::vector<std::string> >::iterator header ;
 
    header = request.get_request_headers().find("Host") ;
    
    if ( std::find( allowed_methods.begin(), allowed_methods.end(), request.get_request_method()  ) ==  allowed_methods.end() )
    {
        std::cout << "request method not allowed" << std::endl ;
        // create function() to format message for method not allowed 
        
    }
    
    if ( header !=  request.get_request_headers().end())
    {
        if ( std::find( server_names.begin(), server_names.end(), *( header->second.begin() )) == server_names.end() )
        {
            std::cout << "host name not allowed" << std::endl ;
            // function() responde with error of host name not allowed
        }
    }

}

/* Getters */

const std::string Response::getResponse( void ) const
{
    (void)this->request ;
    (void)this->server_context ;

    

    return "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<h1 style='color:red;'>Hello from C++ server!</h1>" ;

}

Response::~Response() 
{
    
}
