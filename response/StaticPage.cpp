/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticPage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 12:21:32 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/12 03:23:23 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

bool    Response::path_from_location(std::string & path_of_page, std::string & cgi_extention , std::string & target, std::string & new_target )
{
    if ( ! (_location)->redirect_is_set )
    {
        path_of_page = (_location)->get_root_directory() + "/" + target ;
        if ( ( new_target == target || ( new_target + "/" ) == target ) && _location->get_auto_index() && this->is_folder( path_of_page ) )
        {
            this->respond_list_files( path_of_page, target ) ;
            this->_end_of_response = true ;
            return false ;
        }
        else if ( new_target != target && this->is_folder( path_of_page ) )
            throw 403 ;
        if ( this->is_folder( path_of_page ) )
            path_of_page.append("/" + (_location)->get_index()) ;
    }
    else
    {
        redirection_handler( _location->get_redirection().first, _location->get_redirection().second ) ;
        return false ;
    }
    cgi_extention = _location->get_cgi_extension();
    return true ;
}

bool    Response::path_from_root(std::string & path_of_page, std::string & cgi_extention , std::string & target)
{
    path_of_page = this->server_context.get_root_directory() + "/" + target ;
    if ( target == "/" && this->is_folder( path_of_page )  && this->server_context.get_auto_index() )
    {
        this->respond_list_files( path_of_page, target ) ;
        this->_end_of_response = true ;
        return false ;
    }
    else if ( target == "/" && this->is_folder( path_of_page ) )
        path_of_page +=  this->server_context.get_index() ;
    else if ( target != "/" && this->is_folder( path_of_page ) )
        throw 403 ;
    else
        path_of_page = this->server_context.get_root_directory() + "/" + target ;
    cgi_extention = this->server_context.get_cgi_extension();
    return true ;
}

bool Response::get_path_of_page(std::string & path_of_page, std::string & cgi_extention  )
{
    std::string                     target = this->request.get_request_target() ;
    std::string new_target      = target ;
    _location                   = find_match_more_location( new_target ) ;

    if ( _location )
    {
        return ( this->path_from_location(path_of_page, cgi_extention, target, new_target) );
    }
    else
    {
        return ( this->path_from_root(path_of_page, cgi_extention, target) );
    }
    return true ;
}

void    Response::get_static_page()
{
    std::string                     path_of_page ;
    char                            buffer[ RESP_BUFF ] = {0} ;
    std::string                     cgi_extention ;

    if ( this->_running_post && ! this->_tranfer_encoding )
    {
        this->post_data() ;
        return ;
    }
    if ( ! this->_tranfer_encoding )
    {
        if ( ! this->get_path_of_page( path_of_page, cgi_extention ) )
            return ;
        if ( ! ((this->_location && this->is_allowd_method_in_location() )\
            || (! this->_location && this->is_allowd_method())) )
            throw 405 ;
        if (  this->request.get_request_method() == "DELETE" )
        {
            this->delete_file( path_of_page ) ;
            return ;
        }
        else if ( this->request.get_request_method() == "POST" )
        {
            this->_running_post = true ;
            this->get_static_page();
        }

        this->page_content.open( path_of_page ) ;
        if ( ! this->page_content.is_open() )
            throw 404 ;
    }

    this->page_content.read( buffer, (RESP_BUFF - 1)) ;
    if ( this->page_content.fail() && ! this->page_content.eof() )
        throw 500 ;
    buffer[this->page_content.gcount()] = '\0' ;
    if ( this->page_content.eof() )
        this->_end_of_response = true ;
    else
        this->_tranfer_encoding = true ;
    this->generate_message(buffer, this->page_content.gcount() ) ;
}

void    Response::generate_message( char * content, size_t size )
{
    std::ostringstream ss ;

    ss << std::hex << size ;
    if ( this->_tranfer_encoding )
    {
        if ( this->is_first_message )
        {
            this->message.append("HTTP/1.1 " + default_error_pages.getErrorMsg( this->status ) + "\r\nTransfer-Encoding: chunked\r\nServer: webserv/0.0\n\r\n") ;
            this->is_first_message = false ;
        }
        this->message.append(ss.str()) ;
        this->message.append("\r\n") ;
        this->message.append(content, size) ;
        this->message.append("\r\n") ;
    }
    else
    {
        this->message.append("HTTP/1.1 " + default_error_pages.getErrorMsg( this->status ) + "\r\nServer: webserv/0.0\r\n") ;
        this->message.append("Content-Length: " + ss.str() + "\r\nContent-Type: text/html; charset=utf-8\r\nConnection: keep-alive\r\n\r\n") ; // Content-Type: text/html; charset=utf-8
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
        if ( location )
            return ( location ) ;
        this->remove_last_slash( new_target ) ;
        if ( new_target == "" )
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
    std::stringstream ss ;

    d = opendir(path.c_str()) ;
    if ( !d )
        throw 500 ;
    ls_files.append("<!DOCTYPE html><html lang=\"en\"><head><meta "
    "charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<title>" + target + "</title>"
    "<style>"
    "table{ padding-left: 100px;}"
    "td{ padding: 5px;}"
    "thead{ text-align: left;}"
    "</style>"
    "</head><body><h1> Index of " + target + "</h1> <hr><table><thead><tr><th>Name</th>"
    "<th>Size</th><th>Date Modified</th></tr></thead><tbody>") ;

    while ( (f = readdir( d )) && f != NULL )
        this->append_row( path, target, f, ls_files) ;
    if ( closedir(d) == -1 )
        throw 500 ;
    ls_files.append("</tbody></table><hr><center><h5>webserv</h5></center><hr></body></html>") ;
    ss << ls_files.length() ;
    this->message.append("HTTP/1.1 200 OK\r\nContent-Length: " + ss.str() + "\r\nContent-Type: text/html\r\n\r\n" + ls_files) ;
}

void Response::append_row( std::string  path , std::string target, struct dirent * f, std::string & ls_files )
{
    struct tm * time ;
    char time_str[100] ;
    std::stringstream size ;
    struct stat s ;

    path.append(f->d_name) ;
    if ( stat( path.c_str(), &s ) == -1 )
        throw 404 ;
    time = localtime(&s.st_mtime) ;
    if ( ! time || strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time) == 0 )
        throw 500 ;
    size << f->d_reclen ; // to megabytes 1024^2
    target.append(f->d_name) ;
    ls_files.append("<tr><td><a href='" + target + "'>") ;
    ls_files.append(f->d_name) ;
    ls_files.append("</a></td><td>" + size.str() + " bytes</td><td>") ;
    ls_files.append(time_str) ;
    ls_files.append("</td></tr>") ;
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
