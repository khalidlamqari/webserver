/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticPage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 12:21:32 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/31 22:14:50 by klamqari         ###   ########.fr       */
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
    char                            buffer[ 1024 ] = {0} ;

    target    =                     this->request.get_request_target() ;
    locations =                     this->server_context.get_locations();
    


    
    // std::vector<std::string> paths = _split_(target, '/');
    // std::vector<std::string>::iterator i ;
    LocationContext * location = NULL ;
    std::string new_taget = target ;
    while ( true )
    {
        std::cout << "new_taget " << new_taget << std::endl;
        location =    this->find_location( new_taget ) ;

        if ( location ) // if find location
            break ;
        
        remove_last_slash( new_taget ) ;
        if ( new_taget == "" ) // if all paths removed / / /
            break ;

    }

    if ( location != NULL )
    {
        if ( ! (location)->redirect_is_set )
        {
            path_of_page = (location)->get_root_directory() + "/" + target.substr( location->get_location().length() );
            if ( target.back() == '/' )
                path_of_page.append("/" + (location)->get_index()) ;
        }
        else
        {
            path_of_page = (location)->get_root_directory() + "/" + target ;
        }
    }
    else
    {

        /*
         *    http://127.0.0.1:8888/folder
         **   If the folder exists and directory listing is enabled: Nginx will display the contents of the folder.
         ***  If the folder exists but directory listing is not enabled and no index file is present: Nginx will return a 403 Forbidden error.
         **   If the folder does not exist: Nginx will return a 404 Not Found error.
         *
        */

        if ( target == "/" )
            path_of_page = this->server_context.get_root_directory() + "/" + this->server_context.get_index() ;
        else if ( target.back() ==  '/') // should test nginx
            throw 403 ;
        else
            path_of_page = this->server_context.get_root_directory() + "/" + target;
    }
    
    page_content.open( path_of_page ) ;

    if ( ! page_content.is_open() )
        throw 404 ;

    page_content.read( buffer, 1024 ) ; // TODO: should check if reading is done or not
    buffer[page_content.gcount()] = '\0' ;

    if ( page_content.fail() && ! page_content.eof() )
        throw 500 ;

    this->generate_message(buffer, page_content.gcount() );
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

LocationContext * Response::find_location(const std::string &target)
{
    std::vector<LocationContext>& locations = this->server_context.get_locations();
    
    for (std::vector<LocationContext>::iterator i = locations.begin(); i != locations.end(); ++i)
    {
        if (i->get_location() == target)
        {
            return &(*i);
        }
    }
    return NULL;
}

void Response::remove_last_slash( std::string & target )
{
    size_t end = target.find_last_of('/') ;
    if ( end != std::string::npos )
    {
        if ( target.back() == '/' )
            target.pop_back();
        else
            target = target.substr(0, end + 1) ;
    }
    else
        target = "" ;
}