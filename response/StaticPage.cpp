
# include "Response.hpp"
# include "DefaultInfo.hpp"
#include "../Utils/utils.hpp"

void    Response::format_response()
{
    /* check if return is set in location . and this is the first chunck or message */
    if ( is_first_message && is_redirect(_location) )
    {
        redirection_handler();
    }

    /* */
    else if ( is_first_message && client_info.get_request()->get_method() == "DELETE" )
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

/*
**     301 (Moved Permanently)
**     302 (Found / Moved Temporarily)
**     303 (See Other)
**     307 (Temporary Redirect)
**     308 (Permanent Redirect)
*/
void    Response::redirection_handler()
{
    std::stringstream ss;
    status = _location->get_redirection().status_code;
    std::string target = _location->get_redirection().target;
    if ( target == "" )
        throw (int)status;

    format_start_line();
    set_connection_header(*client_info.get_request(), message, status);

    if ( (status > 300 && status < 304) || 307 == status || 308 == status )
    {
        body_size = 0;
        message.append( "Location: " + target + "\r\nContent-Length: 0\r\n\r\n" );
    }
    else
    {
        body_size = target.length();
        ss << target.length() ;
        message.append("Content-Length: " + ss.str() + "\r\n");
        message.append("\r\n" + target ); // add target to body of response 
    }
    _end_of_response = true ;
}

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

    if (access(_path_.c_str(), R_OK) == -1)
        throw 403;
    
    d = opendir(this->_path_.c_str()) ;
    if ( !d )
        throw 500 ;

    create_html_table(ls_files, _target);

    /* loop on content (files dir ... ) of directory  */
    while ( (f = readdir( d )) && f != NULL )
    {
        if (std::string(f->d_name) != ".")
            append_row( *this, f, ls_files);
    }
    if ( closedir(d) == -1 )
        throw 500 ;
    ls_files += "</tbody></table><hr><center><h5>webserv</h5></center><hr></body></html>" ;

    format_start_line();
    body_size = ls_files.length();
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
    parse_headers();

    format_body(NULL, 0);
}

size_t str_to_size_t(const std::string & str)
{
    size_t num = 0;
    std::stringstream ss;
    ss << str;
    if (ss.fail())
        throw 502;
    ss >> num;

    return num;
}

void set_status_header(Response & response, std::vector<std::string> & arr_headers, std::string & message)
{
    /*  Start line */
    bool is_set = false;
    std::string header_value;
    
    header_value = get_header_value(arr_headers, "Status:", is_set);
    if (!is_set)
        response.format_start_line();
    else
        message += "HTTP/1.1 " + header_value + "\r\n";
}

void check_content_length_header(Response & response, std::vector<std::string> & arr_headers)
{
    bool is_set = false;
    std::string header_value;
    size_t content_length = 0;
    size_t body_length = 0;
    
    header_value = get_header_value(arr_headers, "content-length:", is_set);
    if (is_set)
    {
        trim_white_spaces(header_value, " \t\v\f\b", "\n\r");
        if (!is_all_digits(header_value))
            throw 502;
        content_length = str_to_size_t(header_value);
        body_length = response.get_data_out().length();

        if (content_length > body_length)
            throw  502;
        else if (content_length < body_length)
            response.get_data_out().erase(content_length);
    }
}

void set_content_length(bool & _tranfer_encoding, bool & is_first_message, std::string & message, std::string & data_out)
{
    if (_tranfer_encoding && is_first_message)
    {
        message += "Transfer-Encoding: chunked\r\n";
    }
    else if ( !_tranfer_encoding )
    {
        std::ostringstream ss;
        ss << data_out.length() ;
        message += "Content-Length: " + ss.str() + "\r\n";
    }
}

void Response::parse_headers()
{
    if (!is_first_message)
        return;

    std::string headers = extract_headers(data_out);

    std::vector<std::string> arr_headers = split(headers, "\r\n");

    std::string header_value;

    if (data_out.length() > RESP_BUFF)
        _tranfer_encoding = true;

    /*  Start line */
    set_status_header(*this, arr_headers, message);

    set_connection_header(*client_info.get_request(), message, status);

    check_content_length_header(*this, arr_headers);

    set_content_length(_tranfer_encoding, is_first_message, message, data_out);

    /* set headers in message */
    write_headers_to_msg(arr_headers, message);
    message += "\r\n";

    /* move the offset to the next position (body begin) */
    offset = 0;
    is_first_message = false;
}

void Response::read_cgi_output()
{
    ssize_t n;
    char    buffer[ RESP_BUFF ];

    if (!_cgi_pair_socket)
    {
        // exit(0);
        return ;
    }
    n = read(_cgi_pair_socket->get_ident(), buffer, (RESP_BUFF - 1));
    if ( n == -1 )
    {
        _is_cgi = false;
        status = 500;
    }
    else
    {
        buffer[n] = '\0';
        this->data_out.append(buffer, n);
    }
     /* reset start time  */

    CgiProcess* pr = this->get_cgi_process();
    pr->set_start_time(std::time(0));
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

std::string find_extention(Response & response)
{
    std::map<extension, execPath>::const_iterator end;
    std::map<extension, execPath>::const_iterator it;

    if (response.get_location())
    {
        end = response.get_location()->get_cgi_info().end();
        it = response.get_location()->get_cgi_info().begin();
    }
    else
    {
        end = response.server_context->get_cgi_info().end();
        it = response.server_context->get_cgi_info().begin();
    }
    
    std::string path = response.get_path();
    
    for (; it != end; ++it)
    {
        if ( path.find(it->first + "/") != std::string::npos)
        {
            return it->first;
        }
    }
    return "";
}

void    Response::extract_pathinfo_form_target()
{
    std::string tmp_info;
    std::string tmp_target;
    _cgi_extention = find_extention(*this);

    if (_cgi_extention == "")
        return ;

    size_t pos = _path_.find(_cgi_extention + "/");
    if (pos == std::string::npos)
        return ;
    

    tmp_target = _path_.substr(0, _cgi_extention.length() + pos);
    tmp_info = _path_.substr(_cgi_extention.length() + pos);

    if ( is_file(tmp_target) )
    {
        _path_ = tmp_target;
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
        message += get_content_type(_path_);

        message += "\r\n";

        set_connection_header(*client_info.get_request(), message, status);
        message += "\r\n";
        is_first_message = false;
    }
    
}

void Response::format_body(char * content, size_t size)
{
    std::string body = "";

    if ( client_info.get_request()->get_method() == "HEAD" )
    {
        _end_of_response = true;
        return ;
    }

    /* in case cgi the data stored in string , so a read from string. otherwise i read from the file directly. the data commes in parameters */
    if (_is_cgi)
    {
         /* extract_body function get data from string (data_out) and move the offset to the next position */
        body = extract_body(data_out, offset);
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
        std::ostringstream size_hex;
        body_size += body.length();

        size_hex << std::hex << body.length();
        message.append(size_hex.str() + "\r\n");
        message.append(body + "\r\n");
    }
    else   /* normal body  */
    {
        body_size = body.length();
        message.append(body);
    }
}

bool is_first_prrocessing(bool & is_processed)
{
    if (is_processed)
        return false;

    is_processed = true;
    return true;
}

/*
    set servercontext
    check target
    find location
    check methods
    check is redirect
    init info (upload dir path extention status)
*/

void    Response::process_request()
{
    if (!is_first_prrocessing(this->is_processed))
        return;

    /* 1 set and search for server context by Host header */
    this->server_context = get_server_context(this->client_info);
    client_info.get_request()->max_body_size = server_context->get_max_body_size();
    
    /* should check status of request */
    if (client_info.get_request()->isBadRequest())
    {
        this->status = client_info.get_request()->bad_status_code;
    }

    /* 2 set and checking target */
    set_target(client_info.get_request()->get_target());
    normalize_target(this->_target , this->status);

    /* 3 find location */
    find_match_more_location();

    /* 4 check is allowed method */
    if (this->status == 200 && !this->is_allowed_method())
    {
        this->status = 405;
        return;
    }

    /* 5 check is redirect */
    if (status != 200 )
        return;

    /* 6 extracte and init info ( upload dir path extention status ) */
    if ( _location )
        extract_info_from_location(*this, *_location);
    else
        extract_info_from_server(*this, *(this->server_context));

    /* 7 check is a cgi request & if cgi create socketpair for IPC and open file in tmp */
    set_cgi_requerements(*this, _is_cgi);
}
