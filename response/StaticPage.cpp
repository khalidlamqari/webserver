/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticPage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 12:21:32 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/28 10:50:02 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"
# include "DefaultInfo.hpp"

void    Response::format_response()
{
    /* check if return is set in location . and this is the first chunck or message */
    if ( is_first_message && is_redirect(_location) )
    {
        redirection_handler();
    }

    /* */
    else if ( is_first_message && clientsocket.get_request()->get_method() == "DELETE" )
    {
        delete_file();
    }

    /* check if the request for listing directory content . and this is the first chunck or message */
    else if ( is_first_message && is_directory_list(_path_, _location, *(server_context)) )
    {
        directory_listing();
    }

    /* the data was readed from child process stored in string . and athers data should be readed directly from the file, that why i separate cgi and athers response */
    else if (_is_cgi)
    {
        format_cgi_response();
    }

    /*  */
    else
    {
        format_static_response();
    }
}

void    Response::redirection_handler()
{
    std::stringstream ss;
    status = _location->get_redirection().status_code;
    std::string target = _location->get_redirection().target;
    format_start_line();
    set_connection_header(message, !clientsocket.get_request()->isPersistent(), status);

    if ( (status > 300 && status < 304) || 307 == status || 308 == status )
    {
        message.append( "Location: " + target + "\r\nContent-Length: 0\r\n\r\n" );
    }
    else
    {
        ss << target.length() ;
        message.append("Content-Length: " + ss.str() + "\r\n");
        message.append("\r\n" + target ); // add target to body of response 
    }
    _end_of_response = true ;
}


/* TODO : delete directory and check if delete method in process request  */
void Response::delete_file()
{
    if ( is_file(_path_) )
    {
        remove_file(_path_);
        throw 204 ;
    }
    else if (is_dir(_path_))
    {
        remove_dir_recursive(_path_);
        throw 204;
    }

    throw 404 ;
}

/* this function open directory and  get its content (files directories ... ) one by one and format it in html table  */
void    Response::directory_listing()
{
    std::string ls_files ;
    DIR *d ;
    struct dirent *f ;

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

    format_start_line();
    format_headers(ls_files.length());
    message += ls_files ; /* add body content to message */
    _end_of_response = true ;
}


void Response::format_static_response()
{
    char    buffer[ RESP_BUFF ];
    size_t size = 0;

    open_file(_tranfer_encoding, page_content, _path_);

    read_file(page_content, buffer, size);

    check_end_of_file(page_content, _end_of_response, _tranfer_encoding);

    format_start_line();

    format_headers(size);
    
    format_body(buffer, size);
}

void    Response::format_cgi_response()
{
    // format_start_line();

    parse_headers();

    format_body(NULL, 0);

}


std::string find_header(const std::string & headers, const std::string & header_name)
{
    size_t pos = 0;
    size_t end = 0;

    pos = headers.find(header_name);
    if ( pos == std::string::npos)
        return "";

    end = headers.find("\r\n", pos);

    return (headers.substr(pos + header_name.length(), end - (pos + header_name.length())));
}

void Response::parse_headers()
{
    size_t pos;

    if (!is_first_message)
        return ;

    pos = data_out.find("\r\n\r\n");
    if (pos == std::string::npos)
        throw 500;

    std::string headers = extract_headers(data_out, pos);

    if ( data_out.length() - pos + 4 > RESP_BUFF )
        _tranfer_encoding = true;

    std::string header = find_header(headers, "Status:");
    if (header == "")
        format_start_line();
    else
        message += "HTTP/1.1 " + header + "\r\n";


    header = find_header(headers, "Content-type:");
    if (header != "")
        message += "Content-type:" + header + "\r\n";

    format_headers(data_out.length() - pos - 4);

    // message.append(headers);
    offset += pos + 4;
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

void Response::format_start_line()
{
    if ((_tranfer_encoding && is_first_message) || !_tranfer_encoding)
        message.append("HTTP/1.1 " + default_info.getCodeMsg( status ) + "\r\n" );
}

void  Response::format_headers(size_t size)
{
    if (_tranfer_encoding && is_first_message)
    {
        message += "Transfer-Encoding: chunked";
    }
    else if ( !_tranfer_encoding )
    {
        std::ostringstream ss;
        ss << size ;
        message += "Content-Length: " + ss.str();
    }

    /* shared headers */
    if ((_tranfer_encoding && is_first_message ) || !_tranfer_encoding)
    {
        if (is_first_message && is_directory_list(_path_, _location, *(server_context)))
            message += "\r\nContent-Type: text/html";
        else if (!_is_cgi)
            message += get_content_type(_path_);

        message += "\r\n";

        set_connection_header(message, !clientsocket.get_request()->isPersistent(), status);
        message += "\r\n";
        is_first_message = false;
    }
}

void Response::format_body(char * content, size_t size)
{
    std::string body = "";

    if ( clientsocket.get_request()->get_method() == "HEAD" )
    {
        _end_of_response = true;
        return ;
    }

    /* in case cgi the data stored in string , so a read from string. otherwise i read from the file directly. the data commes in parameters */
    if (_is_cgi)
    {
        body = extract_body(data_out, offset); /* extract_body function get data from string (data_out) and move the offset to the next position */
        offset += body.length();

        /* i set end_of_response = true , in case no data to send */
        if ((_tranfer_encoding && body.length() == 0) || !_tranfer_encoding)
            _end_of_response = true;
    }
    else
    {
        body.append(content, size);
    }

    /* body of tranfer encoding (chunck) */
    if ( _tranfer_encoding )    
    {
        std::ostringstream size_hex ;

        size_hex << std::hex << body.length();
        message.append(size_hex.str() + "\r\n");
        message.append(body + "\r\n");
    }
    else   /* normal body  */
    {
        message.append(body);
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
    set_cgi_requerements(*this, _is_cgi); 

}
