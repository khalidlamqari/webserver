/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticPage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 12:21:32 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/29 17:41:45 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

void    Response::get_static_page()
{
    std::string                     root ;
    std::string                     path_of_page ;
    std::string                     index;
    std::string                     target;
    std::vector<LocationContext>    locations ;
    std::ifstream                   page_content ;
    char               buffer[ 1024 ] = {0} ; // one megabytes

    target = this->request.get_request_target() ;
    locations = this->server_context.get_locations();
    
    // std::vector<LocationContext>::iterator location =  this->find_location( target ) ;

    std::vector<LocationContext>::iterator location ;
    for ( location = locations.begin() ; location != locations.end() ; location++ )
    {
        if ( (location)->get_location() == target )
            break ;
    }

    if ( location != locations.end() )
    {
        if ( ! (location)->redirect_is_set )
        {
            path_of_page = (location)->get_root_directory() + "/" + (location)->get_index() ;
            std::cout << " file path " << path_of_page <<  std::endl ;
            
            page_content.open( path_of_page ) ;

            if ( ! page_content.is_open() )
                throw 404 ;

            page_content.read( buffer, 1024 ) ; // TODO: should check if reading is done or not

            buffer[page_content.gcount()] = '\0' ;
            
            if ( page_content.fail() && ! page_content.eof() )
                throw 500 ;

            this->generate_message(buffer, page_content.gcount() );

        }
        else
        {
            
        }
    }
    

}

/*

HTTP/1.1 200 OK
Server: nginx/1.21.3
Date: Tue, 29 Oct 2024 12:00:00 GMT
Content-Type: text/html
Content-Length: 2567
Connection: keep-alive

*/

void    Response::generate_message( char * content, size_t size )
{
    std::ostringstream ss;
    
    ss << size ;

    this->message.append("HTTP/1.1 200 OK\r\n") ;
    this->message.append("Server: webserv/0.0\r\nContent-Type: text/html\r\n") ;
    this->message.append("Content-Length: " + ss.str() + "\r\n") ;
    this->message.append("Connection: keep-alive\r\n\r\n") ;
    
    this->message.append(content, size);

}

std::vector<LocationContext>::iterator  Response::find_location( const std::string & target )
{
    std::vector<LocationContext>::iterator i ;
    std::vector<LocationContext>    locations ;
    
    locations = this->server_context.get_locations() ;
    for ( i = locations.begin() ; i != locations.end() ; i++ )
    {
        if ( (i)->get_location() == target )
        {
            std::cout << i->get_root_directory() << std::endl;
            return ( i ) ;
        }
    }
    return ( i ) ;
}
