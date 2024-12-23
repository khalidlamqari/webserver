/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticPage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 12:21:32 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/23 13:00:33 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"


/*
    redirect
    autoindex
    file content
*/

static bool is_redirect(LocationContext * location)
{
    if (location && location->redirect_is_set)
        return true;
    return false;
}

static bool is_directory_list(const std::string & path, LocationContext * location, const ServerContext & servercontext)
{
    bool autoindex;

    if (location)
        autoindex = location->get_auto_index();
    else
        autoindex = servercontext.get_auto_index();
    
    if (is_dir( path ) && autoindex)
        return true;
    return false;
}


void    Response::format_response()
{
    if ( !_tranfer_encoding && is_redirect(_location) )
    {
        redirection_handler();
    }
    else if ( !_tranfer_encoding && clientsocket.get_request()->get_method() == "DELETE" )
    {
        delete_file();
    }
    else if ( !_tranfer_encoding && is_directory_list(_path_, _location, *(server_context)) )
    {
        directory_listing();
    }
    else if (_is_cgi )
    {
        format_cgi_msg();
    }
    else
    {
        read_and_format_msg();
    }
}

void    Response::redirection_handler()
{
    std::stringstream ss;
    unsigned short status = _location->get_redirection().status_code;
    std::string msg_or_file = _location->get_redirection().target;
    
    if ( (status > 300 && status < 304) || 307 == status || 308 == status )
    {
        message.append( "HTTP/1.1 "  + default_info.getCodeMsg( status ) + "\r\n" );
        message.append( "Location: " + msg_or_file + "\r\nContent-Length: 0\r\n" );
        if (clientsocket.get_request()->get_is_persistent())
            message.append("Connection: close\r\n\r\n");
        else
            message.append("Connection: keep-alive\r\n\r\n");
    }
    else
    {
        ss << msg_or_file.length() ;
        message.append( "HTTP/1.1 "  + default_info.getCodeMsg( status ) + "\r\n" );
        message.append("Content-Length: " + ss.str() + "\r\n");
        if (clientsocket.get_request()->get_is_persistent())
            message.append("Connection: close\r\n\r\n");
        else
            message.append("Connection: keep-alive\r\n\r\n");
        message.append("\r\n\r\n" + msg_or_file );
    }
    _end_of_response = true ;
}

/* TODO : delete directory and check if delete method in process request  */
void Response::delete_file()
{
    struct stat f ;
    std::cout << "path : " << _path_ << std::endl;
    if ( stat( this->_path_.c_str(), &f ) == -1)
        throw 404 ;

    int status = remove(this->_path_.c_str()) ;
    if ( status == -1 )
        throw 430 ;
    throw 204 ;
}

/* this function open directory and  get its content (files directories ... ) one by one and format it in html table  */
void    Response::directory_listing()
{
    std::string ls_files ;
    DIR *d ;
    struct dirent *f ;
    std::stringstream ss ;

    d = opendir(this->_path_.c_str()) ;
    if ( !d )
        throw 500 ;

    create_html_table(ls_files, _target);

    /* loop on content (files dir ... ) of directory  */
    while ( (f = readdir( d )) && f != NULL )
    {
        append_row( this->_path_, this->_target, f, ls_files);
    }
    if ( closedir(d) == -1 )
        throw 500 ;
    ls_files += "</tbody></table><hr><center><h5>webserv</h5></center><hr></body></html>" ;
    ss << ls_files.length() ;
    set_headers(message, ss.str());
    message += ls_files ; /* add body content to message */
    _end_of_response = true ;
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
        remove_last_slash( target );
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



void    Response::extract_pathinfo_form_target(const std::string & root)
{
    std::string tmp_info;
    std::string tmp_target;

    if (_cgi_extention == "")
        return ;

    size_t pos = _target.find(_cgi_extention + "/");
    if (pos == std::string::npos)
        return ;
    

    tmp_target = _target.substr(0, _cgi_extention.length() + pos);
    tmp_info = _target.substr(_cgi_extention.length() + pos);

    if ( is_file(root + tmp_target) )
    {
        _target = tmp_target;
        _path_info = tmp_info;
    }
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
    /* 1 set and search for server context by Host header */
    this->server_context = get_server_context(this->clientsocket);

    /* TODO should check status of request */
    
    /* 2 set and checking target */
    set_target(clientsocket.get_request()->get_target());
    normalize_target( this->_target , this->status);

    /* 3 find location */
    find_match_more_location();

    /* 4 check is allowed method */
    if (!this->is_allowed_method() && this->status == 200)
    {
        this->status = 405;
        return ;
    }

    /* 5 check is redirect */
    if (_location && _location->redirect_is_set)
        return;

    /* 6 extracte and init info ( upload dir path extention status ) */
    if ( _location )
        extract_info_from_location(*this, *_location);
    else
        extract_info_from_server(*this, *(this->server_context));
    
    /* 7 check is a cgi request & if cgi create socketpair for IPC and open file in tmp */
    set_cgi_requerements(*this, _is_cgi, input_data); /* input_data is a ostream for pass body to child process (cgi) */
    
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
