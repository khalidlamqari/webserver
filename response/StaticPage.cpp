/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticPage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 12:21:32 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/18 06:59:13 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"


static void normalize_target(std::string &target)
{
    if ( target.find("..") == std::string::npos )
        return ;
    std::vector<std::string> directories = _split_(target, '/');
    std::vector<std::string>::iterator it = directories.begin();
    while (it != directories.end())
    {
        if (it == directories.begin() && *it == "..")
            throw 403;
        if (*it != ".." && it + 1 != directories.end() && *(it + 1) == "..")
        {
            it = directories.erase(it, it + 2);
            if (it != directories.begin())
                --it;
        }
        else
            ++it;
    }
    target = "/";
    for (std::vector<std::string>::iterator it = directories.begin() ; it != directories.end() ; ++it) 
        target += "/" + *it;
}

bool    Response::path_from_location( std::string & target, std::string & new_target )//index.php/path/info
{
    if ( ! (_location)->redirect_is_set )
    {
        this->_path_ = (_location)->get_root_directory() + "/" + target ;
        if ( ( new_target == target || ( new_target + "/" ) == target ) && _location->get_auto_index() && this->is_folder( this->_path_ ) )
        {
            this->respond_list_files( target ) ;
            this->_end_of_response = true ;
            return false ;
        }
        else if ( new_target != target && this->is_folder( this->_path_ ) )
            throw 403 ;
        if ( this->is_folder( this->_path_ ) )
            this->_path_.append("/" + (_location)->get_index()) ;
    }
    else
    {
        redirection_handler( _location->get_redirection().first, _location->get_redirection().second ) ;
        return false ;
    }
    // this->_cgi_extention = _location->get_cgi_extension();
    return true ;
}

bool    Response::path_from_root( std::string & target)
{
    this->_path_ = this->server_context.get_root_directory() + "/" + target ;
    if ( target == "/" && this->is_folder( this->_path_ )  && this->server_context.get_auto_index() )
    {
        this->respond_list_files( target ) ;
        this->_end_of_response = true ;
        return false ;
    }
    else if ( target == "/" && this->is_folder( this->_path_ ) )
        this->_path_ +=  this->server_context.get_index() ;
    else if ( target != "/" && this->is_folder( this->_path_ ) )
        throw 403 ;
    // else
    //     this->_path_ = this->server_context.get_root_directory() + "/" + target ;
    // this->_cgi_extention = this->server_context.get_cgi_extension();
    return true ;
}

bool Response::get_path_of_page(  )
{
    // std::string           target = this->request.get_request_target() ;
    // normalize_target( target ) ;
    std::string new_target      = this->_target ;
    _location                   = find_match_more_location( new_target ) ;

    if ( _location )
        return ( this->path_from_location(this->_target, new_target) );
    else
        return ( this->path_from_root(this->_target) );
    return true ;
}

void    Response::get_static_page()
{
    if ( this->_running_post )
    {
        this->post_data() ;
        return ;
    }
    else if ( ! this->_tranfer_encoding )
    {
        if ( ! this->get_path_of_page( ) )
            return ;

        if ( ! ((this->_location && this->is_allowd_method_in_location() )\
            || (! this->_location && this->is_allowd_method())) )
            throw 405 ;

        if ( this->request.get_request_method() == "DELETE" )
        {
            this->delete_file(  ) ;
            return ;
        }
        else if ( !this->_is_cgi && this->request.get_request_method() == "POST" )
        {
            this->_running_post = true ;
            this->post_data() ; // upload data
            return ;
        }
        else if ( this->_is_cgi )
            this->execute_cgi() ;
    }
    
    if (this->_is_cgi && this->exit_stat == 0 )
        this->read_cgi_output() ;
    else
        this->read_and_format_msg() ;
}

void Response::read_cgi_output()
{
    ssize_t n ;

    if ( ! this->_tranfer_encoding )
    {
        if ( close(this->s_fds[1]) == -1)
            throw 500 ;
    }
    char    buffer[ RESP_BUFF ] = {0} ;
    n = read(this->s_fds[0], buffer, (RESP_BUFF - 1)) ;
    if ( n == -1 )
        throw 500 ;
    buffer[n] = '\0' ;
    if ( n == 0 )
        this->_end_of_response = true ;
    else
        this->_tranfer_encoding = true ;
    this->generate_message(buffer, n ) ;
}

void Response::read_and_format_msg()
{
    if ( !this->_tranfer_encoding )
    {
        this->page_content.open( this->_path_ ) ;
        if ( ! this->page_content.is_open() )
            throw 404 ;
    }
    char    buffer[ RESP_BUFF ] = {0} ;
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

std::string get_content_type( const std::string & file_name )
{
    std::string imgs[] = {".png",".avif", ".gif", ".webp", ".dmp", ".apng"} ;
    std::string jpeg[] = {".jpg", ".jpeg", ".jfif", ".pjpeg", ".pjp"} ;
    std::string ico[] = {".ico", ".cur"} ;
    std::string texts[] = {".txt", ".html", ".htm", ".css", ".js"} ;
    size_t size = sizeof(texts) / sizeof(texts[0]) ;
    for (size_t i = 0; i < size ; ++i)
    {
        if ( file_name.find(texts[i], file_name.length() - texts[i].length()) != std::string::npos )
        {
            std::cout << " enter here |"  << texts[i] << "|" << std::endl;
            if ( texts[i] == ".js" || texts[i] == ".javascript")
                return ( "\r\nContent-Type: text/javascript; charset=utf-8") ;
            return ( "\r\nContent-Type: text/" +  texts[i].erase(0, 1) + "; charset=utf-8") ;
        }
    }
    size = sizeof(imgs) / sizeof(imgs[0]) ;
    for (size_t i = 0; i < 6; ++i)
    {
        if ( file_name.find(imgs[i], file_name.length() - imgs[i].length()) != std::string::npos )
            return ( "\r\nContent-Type: image/" +  imgs[i].erase(0, 1) ) ;
    }
    size = sizeof(jpeg) / sizeof( jpeg[0]) ;
    for ( size_t i = 0; i < size ; ++i)
    {
        if ( file_name.find(jpeg[i], file_name.length() - jpeg[i].length()) != std::string::npos)
            return ( "\r\nContent-Type: image/jpeg" ) ;
    }
    size = sizeof(ico) / sizeof(ico[0]) ;
    for (size_t i = 0; i < ico->length(); ++i)
    {
        if ( file_name.find(ico[i] ,file_name.length() - ico[i].length()) != std::string::npos )
            return ( "\r\nContent-Type: image/x-icon" ) ;
    }
    if ( file_name.find(".mp4", file_name.length() - 4) != std::string::npos )
        return ( "\r\nContent-Type: video/mp4" ) ;
        
    if ( file_name.find(".php", file_name.length() - 4) != std::string::npos )
        return ( "\r\nContent-Type: text/html" ) ;
    return ("") ;
}

void    Response::generate_message( char * content, size_t size )
{
    std::ostringstream ss ;

    ss << std::hex << size ;
    if ( this->_tranfer_encoding )
    {
        if ( this->is_first_message )
        {
            this->message.append("HTTP/1.1 " + default_error_pages.getErrorMsg( this->status ) +\
            "\r\nTransfer-Encoding: chunked" + get_content_type(this->_path_) + "\r\nServer: webserv/0.0\n\r\n") ;
            this->is_first_message = false ;
            std::cout << "message : " << this->message << std::endl;
        }
        this->message.append(ss.str()) ;
        this->message.append("\r\n") ;
        this->message.append(content, size) ;
        this->message.append("\r\n") ;
    }
    else
    {
        this->message.append("HTTP/1.1 " + default_error_pages.getErrorMsg( this->status ) + "\r\nServer: webserv/0.0\r\n") ;
        this->message.append("Content-Length: " + ss.str() + get_content_type(this->_path_) + "\r\nConnection: keep-alive\r\n\r\n") ;
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

void    Response::respond_list_files( const std::string & target )
{
    std::string ls_files ;
    DIR *d ;
    struct dirent *f ;
    std::stringstream ss ;

    d = opendir(this->_path_.c_str()) ;
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
        this->append_row( this->_path_, target, f, ls_files) ;
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

void                                    Response::get_pathinfo_form_target()
{
    size_t pos = this->_target.find(".php/");
    if (pos == std::string::npos)
        return ;

    this->_path_info = this->_target.substr(4 + pos);
    this->_target    = this->_target.substr(0, 4 + pos);
    std::cout << "path info : " << this->_path_info << "\nnew target : " << this->_target << std::endl;

}

bool                                    Response::process_target()
{
    this->_target = this->request.get_request_target() ;
    normalize_target( this->_target ) ;
    this->get_pathinfo_form_target();
    std::string new_target      = this->_target ;
    _location                   = find_match_more_location( new_target ) ;

    if ( _location )
    {
        if ( ! (_location)->redirect_is_set )
        {
            this->_path_ = (_location)->get_root_directory() + "/" + this->_target ;
            if ( new_target != this->_target && this->is_folder( this->_path_ ) )
                return false ;
            if ( this->is_folder( this->_path_ ) )
                this->_path_.append("/" + (_location)->get_index()) ;
        }
        else
            return false ;
        this->_cgi_extention = _location->get_cgi_extension();
        return true ;
    }
    else
    {
        this->_path_ = this->server_context.get_root_directory() + "/" + this->_target ;
        if ( this->_target == "/" && this->is_folder( this->_path_ )  && this->server_context.get_auto_index() )
        {
            return false ;
        }
        else if ( this->_target == "/" && this->is_folder( this->_path_ ) )
            this->_path_ +=  this->server_context.get_index() ;
        else if ( this->_target != "/" && this->is_folder( this->_path_ ) )
            return false ;
            
        this->_cgi_extention = this->server_context.get_cgi_extension();
        return true ;
    }
    
    return true ;
}