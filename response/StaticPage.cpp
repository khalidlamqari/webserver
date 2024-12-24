/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticPage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 12:21:32 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/24 20:53:30 by klamqari         ###   ########.fr       */
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

void open_file(const bool & tranfer_encoding, std::ifstream & page_content, const std::string & path)
{
    /* open file once */
    if ( !tranfer_encoding )
    {
        page_content.open( path );
        if ( ! page_content.is_open() )
            throw 404 ;
    }
}

void read_file(std::ifstream & page_content, char *buffer, size_t & size)
{
    page_content.read( buffer, (RESP_BUFF - 1)) ;
    if ( page_content.fail() && ! page_content.eof() )
        throw 500 ;
    buffer[page_content.gcount()] = '\0';
    
    size = page_content.gcount();
}

/* in case large file eof = false and tranfer chuncks by chunck */
void check_end_of_file(std::ifstream & page_content, bool & end_of_response, bool & tranfer_encoding )
{
    if ( page_content.eof())
        end_of_response = true;
    else
        tranfer_encoding = true;

    if ( tranfer_encoding && page_content.gcount() != 0 )
        end_of_response = false;
}


void    Response::format_response()
{
    /* check if return is set in location .  */
    if ( is_first_message && is_redirect(_location) )
    {
        redirection_handler();
    }
    
    /* */
    else if ( is_first_message && clientsocket.get_request()->get_method() == "DELETE" )
    {
        delete_file();
    }

    /* */
    else if ( is_first_message && is_directory_list(_path_, _location, *(server_context)) )
    {
        directory_listing();
    }
    /* */
    else if (_is_cgi)
    {
        format_cgi_response();
    }
    /* */
    else
    {
        format_static_response();
    }
}

void    Response::redirection_handler()
{
    std::cout << "redirect" << std::endl;
    std::stringstream ss;
    unsigned short status = _location->get_redirection().status_code;
    std::string target = _location->get_redirection().target;

    format_start_line();
    
    if ( !clientsocket.get_request()->get_is_persistent() )
        message.append("Connection: close\r\n");
    else
        message.append("Connection: keep-alive\r\n");

    if ( (status > 300 && status < 304) || 307 == status || 308 == status )
    {
        message.append( "Location: " + target + "\r\nContent-Length: 0\r\n\r\n" );
    }
    else
    {
        ss << target.length() ;
        message.append("Content-Length: " + ss.str() + "\r\n");
        message.append("\r\n" + target );
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
        std::ostringstream ss ;
        ss << size ;
        message += "Content-Length: " + ss.str();
    }

    /* shared headers */
    if ((_tranfer_encoding && is_first_message ) || !_tranfer_encoding)
    {
        if (!_is_cgi)
            message += get_content_type(_path_);
        if ( !clientsocket.get_request()->get_is_persistent() || (status >= 400 && status <= 599))
            message += ("\r\nConnection: close\r\n");
        else
            message += ("\r\nConnection: keep-alive\r\n");
        
        if (!_is_cgi)
            message += ("\r\n");
        is_first_message = false;
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
    format_start_line();

    parse_headers();

    format_body(NULL, 0);
}

void Response::parse_headers()
{
    size_t pos;

    if (!is_first_message)
        return ;

    pos = data_out.find("\r\n\r\n");
    if (pos == std::string::npos)
        throw 500;

    if ( data_out.length() - pos + 4 > RESP_BUFF )
        _tranfer_encoding = true;

    format_headers(data_out.length() - pos - 4);

    message.append(extract_headers(data_out, pos));
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
