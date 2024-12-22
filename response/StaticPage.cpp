/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticPage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 12:21:32 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/22 13:13:27 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

bool    Response::path_from_location()
{
    if ((_location)->redirect_is_set) //  && (is_existe(this->_location)
    {
        std::cout << "path : " << _path_ << std::endl;
        redirection_handler(_location->get_redirection().status_code, _location->get_redirection().target);
        return false ;
    }
    if (is_file(this->_path_))
    {
        return true;
    }
    else if (is_dir( this->_path_ ) && _location->get_auto_index())
    {
        this->respond_list_files();
        this->_end_of_response = true ;
        return false ;
    }
    else if (is_dir(this->_path_))
    {
        throw 403;
    }
    return true ;
}

bool    Response::path_from_root()
{
    if (is_file(this->_path_))
    {
        return true;
    }
    if (is_dir( this->_path_ ) && this->server_context->get_auto_index())
    {
        this->respond_list_files();
        this->_end_of_response = true;
        return false;
    }
    else if (is_dir( this->_path_ ))
    {
        throw 403;
    }
    return true;
}

bool Response::get_path_of_page()
{
    if ( _location )
    {   
        return (this->path_from_location());
    }
    else
    {
        return (this->path_from_root());
    }
    return true ;
}

void    Response::get_static_page()
{
    if ( !this->_tranfer_encoding && !this->get_path_of_page()) /* return false in case "List directory or redirect to another location" */
    {   
        return ;
    }
    if ( !this->_tranfer_encoding && !this->is_allowed_method())
    {
        throw 405 ;
    }
    if ( !this->_tranfer_encoding && this->clientsocket.get_request()->get_method() == "DELETE" )
    {
        this->delete_file() ;
        return ;
    }
    if (this->_is_cgi )
    {
        this->format_cgi_msg();
    }
    else
    {
        this->read_and_format_msg();
    }
}

void    Response::format_cgi_msg()
{
    if (!this->is_parsed)   /* in case tranfer encoding send headers only in first chunck */
    {
        this->parse_headers();
        this->get_response_body();
    }
    else
    {
        this->get_response_body();
    }
}

std::string extract_headers(const std::string & unparsed_content, size_t pos)
{
    return ( unparsed_content.substr(0, pos + 4) );
}

std::string extract_body(const std::string & unparsed_content, size_t pos)
{
    return (unparsed_content.substr(pos, RESP_BUFF));
}

void Response::parse_headers()
{
    std::string body;
    std::ostringstream ss ;
    std::ostringstream len ;
    size_t pos;

    pos = data_out.find("\r\n\r\n");
    if (pos == std::string::npos)
    {
        this->is_parsed = false;
        throw 500;
    }
    else
    {
        if ( this->data_out.length() - pos + 4 > RESP_BUFF ) // if body > RES_...
            this->_tranfer_encoding = true;

        this->message.append("HTTP/1.1 " + default_info.getCodeMsg( this->status ) + "\r\n");
        if (this->clientsocket.get_request()->get_is_persistent() || (this->status >= 400 && this->status <= 599))
            this->message.append("Connection: close\r\n");
        else
            this->message.append("Connection: keep-alive\r\n");
        if ( !this->_tranfer_encoding)
        {
            len << (this->data_out.length() - pos - 4);
            this->message.append("Content-Length: " + len.str() + "\r\n");
        }
        else
        {
            this->message.append("Transfer-Encoding: chunked\r\n");
        }
        this->message.append(extract_headers(data_out, pos));
        this->offset += pos + 4;

        this->is_first_message = false;
        this->is_parsed = true;
    }
}

void Response::get_response_body()
{
    std::ostringstream ss ;
    std::string body = extract_body(this->data_out, this->offset);
    this->offset += body.length();

    if (this->_tranfer_encoding)
    {
        ss << std::hex << body.length() ;
        this->message.append(ss.str());
        this->message.append("\r\n");
        this->message.append(body);
        this->message.append("\r\n");
        if ( body.length() == 0 )
            this->_end_of_response = true;
    }
    else
    {
        this->message.append(body);
        this->_end_of_response = true;
    }
}

void Response::read_cgi_output()
{
    ssize_t n ;
    char    buffer[ RESP_BUFF ] ;

    n = read(this->s_fds[0], buffer, (RESP_BUFF - 1)) ;
    if ( n == -1 )
    {
        throw 500 ;
    }
    buffer[n] = '\0';
    this->data_out.append(buffer, n);
}

void Response::read_and_format_msg()
{
    char    buffer[ RESP_BUFF ]  ;

    if ( !this->_tranfer_encoding )
    {
        this->page_content.open( this->_path_ );
        if ( ! this->page_content.is_open() )
            throw 404 ;
    }
    this->page_content.read( buffer, (RESP_BUFF - 1)) ;
    if ( this->page_content.fail() && ! this->page_content.eof() )
        throw 500 ;

    buffer[this->page_content.gcount()] = '\0';
    if ( this->page_content.eof())
        this->_end_of_response = true;
    else
        this->_tranfer_encoding = true;

    if ( this->_tranfer_encoding && this->page_content.gcount() != 0 )
        this->_end_of_response = false;

    this->generate_message( buffer, this->page_content.gcount() );
}

void    Response::generate_message( char * content, size_t size )
{
    std::ostringstream ss ;
    if ( this->_tranfer_encoding )
    {
        ss << std::hex << size ;
        if ( this->is_first_message )
        {
            this->tranfer_encod_headers();
        }
        this->message.append(ss.str());
        this->message.append("\r\n");
        this->message.append(content, size);
        this->message.append("\r\n");
    }
    else
    {
        this->normall_headers( content, size );
    }
}

void Response::find_match_more_location()
{
    std::string target = this->_target;
    while ( true )
    {
        _location = this->find_location( target );
        if ( _location )
        {   
            return ;
        }
        this->remove_last_slash( target );
        if ( target == "" )
        {   
            return ;
        }
    }
}


LocationContext * Response::find_location(const std::string &target)
{
    const std::vector<LocationContext> & locations = this->server_context->get_locations();

    for (std::vector<LocationContext>::const_iterator i = locations.begin(); i != locations.end(); ++i)
    {
        if (i->get_location() == target )
        {
            return (LocationContext *)&(*i) ;
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
    {   
        target = "" ;
    }
}

void    Response::respond_list_files()
{
    std::string ls_files ;
    DIR *d ;
    struct dirent *f ;
    std::stringstream ss ;

    d = opendir(this->_path_.c_str()) ;
    if ( !d )
        throw 500 ;
    ls_files.append("<!DOCTYPE html><html lang=\"en\"><head><meta "
    "charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width"
    ", initial-scale=1.0\"><title>" + this->_target + "</title>"
    "<style>table{ padding-left: 100px;}td{ padding: 5px;}"
    "thead{ text-align: left;}</style></head><body><h1> Index of " 
    + this->_target + "</h1> <hr><table><thead><tr><th>Name</th>"
    "<th>Size</th><th>Date Modified</th></tr></thead><tbody>") ;

    while ( (f = readdir( d )) && f != NULL )
    {
        append_row( this->_path_, this->_target, f, ls_files);
    }
    if ( closedir(d) == -1 )
        throw 500 ;
    ls_files.append("</tbody></table><hr><center><h5>webserv</h5></center><hr></body></html>") ;
    ss << ls_files.length() ;
    this->message.append("HTTP/1.1 200 OK\r\nContent-Length: " + ss.str() \
                        + "\r\nContent-Type: text/html\r\n\r\n" + ls_files) ;
}

void Response::append_row( std::string  path , std::string target, struct dirent * f, std::string & ls_files )
{
    std::stringstream size ;
    struct stat s;

    if (path.back() != '/')
        path.append("/");
    if (target.back() != '/')
        target.append("/");
    path.append(f->d_name);
    if ( stat(path.c_str(), &s ) == -1)
        throw 404;

    size << f->d_reclen;
    target.append(f->d_name);
    ls_files.append("<tr><td><a href='" + target + "'>");
    ls_files.append(f->d_name);
    ls_files.append("</a></td><td>" + size.str() + " bytes</td><td>");
    ls_files.append(get_time(s.st_mtime));
    ls_files.append("</td></tr>");
}

void    Response::get_pathinfo_form_target()
{
    size_t pos = this->_target.find(".php/");
    if (pos == std::string::npos)
        return ;

    this->_path_info = this->_target.substr(4 + pos);
    this->_target    = this->_target.substr(0, 4 + pos);
}

// ADD NEW
static  const ServerContext * get_server_context(ClientSocket & clientsocket)
{
    std::string host = clientsocket.get_request()->get_headers().find("HOST")->second ; // TODO : store host separatly in the request

    for (std::vector<const ServerContext*>::const_iterator i = clientsocket.get_servers().begin() ; i != clientsocket.get_servers().end() ; ++i)
    {
        std::vector<std::string>::const_iterator b = (*i)->get_server_names().begin();
        std::vector<std::string>::const_iterator e = (*i)->get_server_names().end();

        for ( std::vector<std::string>::const_iterator j = b ; j != e ; ++j)
        {
            if ( host == *j )
            {
                return ( *i );
            }
        }
    }
    return (clientsocket.get_servers().front()) ;
}

static void extract_info_from_location(Response & response, LocationContext & location)
{
    response.set_upload_dir(location.get_upload_dir());
    response.set_path(location.get_root_directory() + response.get_target());
    response.set_cgi_extention(location.get_cgi_extension());

    /* if the target is a directory. check if insid it a index . if index existe concatinat it with the path */
    if (is_dir(response.get_path()) && is_file(response.get_path() + "/" + location.get_index()))
        response.set_path(response.get_path() + "/" + location.get_index());
    else if (is_dir(response.get_path()) && !location.get_auto_index())  /* if this path is a directory and autoindex off . that means error 403 (forbidden) */
        response.set_status(403);
    else if (!is_existe(response.get_path()))
        response.set_status(404);

}

static void extract_info_from_server(Response & response,  const ServerContext & servercontext)
{
    response.set_upload_dir(servercontext.get_upload_dir());
    response.set_path(servercontext.get_root_directory() + response.get_target());
    response.set_cgi_extention(servercontext.get_cgi_extension());

    /* if the target is a directory. check if insid it a index . if index existe concatinat it with the path */
    if (is_dir(response.get_path()) && is_file(response.get_path() + "/" + servercontext.get_index()))
        response.set_path(response.get_path() + "/" + servercontext.get_index());
    else if (is_dir(response.get_path()) && !servercontext.get_auto_index())  /* if this path is a directory and autoindex off . that means error 403 (forbidden) */
        response.set_status(403);
    else if (!is_existe(response.get_path()))
        response.set_status(404);
}

/*
    set servercontext
    check target
    find location
    check methods
    check is redirect
    init info (upload dir path extention status)
*/

void    Response::process_requset()
{
    this->server_context = get_server_context(this->clientsocket);

    /* 1 set and checking target */
    set_target(clientsocket.get_request()->get_target());
    normalize_target( this->_target , this->status);
    get_pathinfo_form_target();

    /* 2 find location */
    find_match_more_location();

    /* 3 check is allowed method */
    if (!this->is_allowed_method() && this->status == 200)
    {
        this->status = 405;
        return ;
    }

    /* 4 check is redirect */
    if (_location && _location->redirect_is_set)
        return;

    /* 5 extracte and init info ( upload dir path extention status ) */
    if ( _location )
        extract_info_from_location(*this, *_location);
    else
        extract_info_from_server(*this, *(this->server_context));
}




void    Response::normall_headers(char * content, size_t size )
{
    std::ostringstream ss ;
    ss << size ;
    this->message.append("HTTP/1.1 " + default_info.getCodeMsg( this->status ) 
                                        + "\r\nServer: webserv/0.0\r\n");

    this->message.append("Content-Length: " + ss.str()
                                            + get_content_type(this->_path_)
                                            + "\r\n");

    if (this->clientsocket.get_request()->get_is_persistent() || (this->status >= 400 && this->status <= 599))
    {
        this->message.append("Connection: close\r\n");
    }
    else
        this->message.append("Connection: keep-alive\r\n");
    this->message.append("\r\n");
    if (  this->clientsocket.get_request()->get_method() != "HEAD" )
        this->message.append(content, size);
}

void    Response::tranfer_encod_headers()
{
    this->message.append("HTTP/1.1 " + default_info.getCodeMsg( this->status )
                                        + "\r\nTransfer-Encoding: chunked"
                                        + get_content_type(this->_path_) + "\r\n");
    if (this->clientsocket.get_request()->get_is_persistent() || (this->status >= 400 && this->status <= 599))
        this->message.append("Connection: close\r\n");
    else
        this->message.append("Connection: keep-alive\r\n");
    this->message.append("\r\n");

    this->is_first_message = false;
}
