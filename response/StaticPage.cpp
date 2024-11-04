/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticPage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 12:21:32 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/05 00:33:15 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/main.hpp"
# include <sys/stat.h>
# include <dirent.h>
#include <time.h>
/*
    *    http://127.0.0.1:8888/folder
    **   If the folder exists and directory listing is enabled: Nginx will display the contents of the folder.
    ***  If the folder exists but directory listing is not enabled and no index file is present: Nginx will return a 403 Forbidden error.
    **   If the folder does not exist: Nginx will return a 404 Not Found error.
    *
*/


void Response::get_path_of_page(std::string & path_of_page )
{
    std::string                     target ;
    target                      =   this->request.get_request_target() ;
    std::string new_target      = target ;
    _location  = find_match_more_location( new_target ) ;

    if ( _location != NULL )
    {
        if ( ! (_location)->redirect_is_set )
        {
            // path_of_page = (_location)->get_root_directory() + "/" + target.substr( _location->get_location().length() ) ;
            path_of_page = (_location)->get_root_directory() + "/" + target ;
            
            if ( ( new_target == target || ( new_target + "/" ) == target ) && _location->get_auto_index() && this->is_folder( path_of_page ) )
            {
                this->respond_list_files( path_of_page, target ) ;
                return ;
            }
            else if ( new_target != target && this->is_folder( path_of_page ) )
                throw 403 ;

            if ( this->is_folder( path_of_page ) )
                path_of_page.append("/" + (_location)->get_index()) ;
        }
        else // TODO: handle redirections(return)
        {
            redirection_handler( _location->get_redirection().first, _location->get_redirection().second ) ;
            return ;
        }
    }
    else
    {
        path_of_page = this->server_context.get_root_directory() + "/" + target ;
        if ( target == "/" ) // http://127.0.0.1:8888/folder
            path_of_page +=  this->server_context.get_index() ;
        else if ( this->is_folder( path_of_page ) ) // should test nginx
            throw 403 ;
        else
            path_of_page = this->server_context.get_root_directory() + "/" + target ;
    }
}

void    Response::get_static_page()
{
    std::string                     path_of_page ;
    char                            buffer[ RESP_BUFF ] = {0} ;

    if ( ! this->_tranfer_encoding )
    {
        this->get_path_of_page( path_of_page );
        std::cout << "path_of_page " << path_of_page << std::endl;
        this->page_content.open( path_of_page ) ;
        if ( ! this->page_content.is_open() )
            throw 404 ;
    }
    this->page_content.read( buffer, RESP_BUFF - 1 ) ; // TODO: should check if reading is done or not
    if ( this->page_content.fail() && ! this->page_content.eof() )
        throw 500 ;

    buffer[this->page_content.gcount()] = '\0' ;
    if ( this->page_content.eof())
        this->_end_of_response = true ;
    else
        this->_tranfer_encoding = true ;
    this->generate_message(buffer, this->page_content.gcount() ) ;
}

/*

HTTP/1.1 200 OK
Server: nginx/1.21.3
Date: Tue, 29 Oct 2024 12:00:00 GMT
Content-Type: text/html
Content-Length: 2567
Connection: keep-alive


HTTP/1.1 200 OK
Transfer-Encoding: chunked
Content-Type: text/html
*/

void    Response::generate_message( char * content, size_t size )
{
    std::ostringstream ss ;

    ss << size ;
    if ( this->_tranfer_encoding )
    {
        if ( this->is_first_message ) // send headers first
        {
            this->message.append("HTTP/1.1 " + default_error_pages.getErrorMsg( this->status ) + "\r\nTransfer-Encoding: chunked\r\nServer: webserv/0.0\r\nContent-Type: text/html\r\n\r\n") ;
            this->is_first_message = false ;
        }

        this->message.append(ss.str() + "\r\n") ;
        this->message.append(content, size) ;
        this->message.append("\r\n") ;
    }
    else
    {
        this->message.append("HTTP/1.1 " + default_error_pages.getErrorMsg( this->status ) + "\r\nServer: webserv/0.0\r\nContent-Type: text/html\r\n") ;
        this->message.append("Content-Length: " + ss.str() + "\r\nConnection: keep-alive\r\n\r\n") ;
        this->message.append(content, size) ;
    }
}

LocationContext * Response::find_match_more_location( std::string & new_target )
{
    LocationContext * location = NULL ;
    location = this->find_exact_location( new_target ) ;
    if ( location )
        return ( location ) ;

    while ( true )
    {
        location =    this->find_location( new_target ) ;
        if ( location ) // if find location
            return ( location ) ;

        this->remove_last_slash( new_target ) ;
        if ( new_target == "" ) // if all paths removed
            return ( NULL ) ;
    }
    return ( NULL ) ;
}

LocationContext * Response::find_exact_location(const std::string &target)
{
    std::vector<LocationContext> & locations = this->server_context.get_locations() ;

    for (std::vector<LocationContext>::iterator i = locations.begin(); i != locations.end(); ++i)
    {
        if (i->get_location() == target && i->is_exact_location() )
        {
            return &(*i) ;
        }
    }
    return ( NULL );
}

LocationContext * Response::find_location(const std::string &target)
{
    std::vector<LocationContext> & locations = this->server_context.get_locations();

    for (std::vector<LocationContext>::iterator i = locations.begin(); i != locations.end(); ++i)
    {
        if (i->get_location() == target && ! i->is_exact_location() )
        {
            return &(*i) ;
        }
    }
    return ( NULL );
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

void    Response::respond_list_files( const std::string & path , const std::string & target )
{
    std::string ls_files ;
    DIR *d ;
    struct dirent *f ;
    struct stat s ;
    struct tm *mod_time ;
    std::stringstream ss ;
    std::stringstream size ;
    std::string f_path ;
    std::string new_target ;
    
    char time_str[100];
    
    d = opendir(path.c_str()) ;
    if ( !d )
        throw 500 ;

    ls_files.append("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<title>" + target + "</title>"
    "<style>"
    "table{ padding-left: 100px;}"
    "td{ padding: 5px;}"
    "thead{ text-align: left;}"
    "</style>"
    "</head><body><h1> Index of " + target + "</h1> <hr><table><thead><tr><th>Name</th><th>Size</th><th>Date Modified</th></tr></thead><tbody>");

    while ( (f = readdir( d )) && f != NULL )
    {
        new_target = target ;
        f_path = path ;
        f_path.append(f->d_name) ;
        
        if ( stat( f_path.c_str(), &s ) == -1 )
            throw 500 ;
        mod_time = localtime(&s.st_mtime);
        
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", mod_time);

        size << f->d_reclen ; // to megabytes 1024^2
        new_target.append(f->d_name) ;
        ls_files.append("<tr><td><a href='" + new_target + "'>") ;
        ls_files.append(f->d_name) ;
        ls_files.append("</a></td><td>") ;
        ls_files.append(size.str()) ;
        ls_files.append(" bytes</td><td>") ;
        ls_files.append(time_str) ;
        ls_files.append("</td></tr>") ;
    }

    ls_files.append("</tbody></table><hr><center><h5>webserv</h5></center><hr></body></html>");
    ss << ls_files.length() ;

    this->message.append("HTTP/1.1 200 OK\r\nContent-Length: " + ss.str() + "\r\nContent-Type: text/html\r\n\r\n" + ls_files) ;
    std::cout << this->message << std::endl ;

}

bool    Response::is_folder( const std::string & path )
{
    struct stat s ;

    if ( stat( path.c_str(), &s ) == -1)
        throw 404 ;
    if ( S_IFDIR & s.st_mode )
        return ( true ) ;
    return ( false ) ;
}
