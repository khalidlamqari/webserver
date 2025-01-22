
#include "includes/main.hpp"
# include "DefaultInfo.hpp"

void trim_white_spaces(std::string & header_value, const std::string & allowedWS, const std::string & forbidenWS) // ok
{
    for (size_t i = 0; i < header_value.length() && isspace(header_value[i]); i++)
    {
        if (allowedWS.find(header_value[i]) != std::string::npos)
        {
            header_value.erase(i, 1);
            i--;
        }
		else if (forbidenWS.find(header_value[i]) != std::string::npos)
			throw 502;
    }

	for (size_t i = header_value.length(); i > 0 && isspace(header_value[i]); i--)
    {
        if (allowedWS.find(header_value[i]))
        {
            header_value.erase(i, 1);
            i++;
        }
		else if (forbidenWS.find(header_value[i]))
			throw 502;
    }
}

void to_upper(std::string & str)
{
    for (size_t i = 0; i < str.length(); ++i)
    {
        str[i] = toupper(str[i]);
    }
}

bool is_name_in_header(std::string header, std::string header_name)
{
    size_t pos = 0;
    to_upper(header);
    to_upper(header_name);

    pos = header.find(header_name);
    if (pos != 0 || pos == std::string::npos)
        return false;

    return ( true );
}

std::string get_header_value(const std::vector<std::string> & headers, const std::string & header_name, bool & is_set)
{
    std::vector<std::string>::const_iterator it = headers.begin();
    std::vector<std::string>::const_iterator end = headers.end();
    
    for (; it != end ; ++it)
    {
        if (is_name_in_header(*it, header_name))
        {
            is_set = true;
            return ((*it).substr(header_name.length()));
        }
    }
    is_set = false;
    return "";
}

void write_headers_to_msg(const std::vector<std::string> & headers, std::string & message)
{
    /* set headers in response */
    std::vector<std::string>::const_iterator it = headers.begin();
    std::vector<std::string>::const_iterator end = headers.end();
    bool is_content_type_exist = false;

    for (; it != end; ++it)
    {
        if (is_name_in_header(*it, "Content-Type:"))
            is_content_type_exist = true;

        if (!is_name_in_header(*it, "Status:") && !is_name_in_header((*it), "content-length:"))
            message += (*it) + "\r\n";
    }

    /* if no content-type header that means an error */
    if (!is_content_type_exist)
        throw 500;
}

void remove_file(const std::string & path)
{
    if (remove(path.c_str()) == -1 )
        throw 403 ;
}

void remove_dir_recursive(const std::string & path)
{
    DIR * dir;
    struct  dirent *file;

    dir = opendir(path.c_str());
    if (!dir)
        throw 403;

    while ((file = readdir(dir)) && file != NULL )
    {
        if (std::string(file->d_name) == ".." || std::string(file->d_name) == ".")
            continue;

        if ( is_file(path + "/" + file->d_name ))
            remove_file(path + "/" + file->d_name);

        else if (is_dir(path + "/" + file->d_name))
            remove_dir_recursive(path + "/" + file->d_name);
    }
    if ( closedir(dir) == -1 )
        throw 500 ;
        
    remove_file(path);
}

void    set_connection_header(Request & request, std::string & message, unsigned short & status)
{
    if ( !request.isPersistent() || (status >= 400 && status <= 599))
    {
        message += ("Connection: close\r\n");
        request.markAsPersistent(false);
    }
    else
        message += ("Connection: keep-alive\r\n");
}

bool is_directory_list(const std::string & path, LocationContext * location, const ServerContext & servercontext)
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
        if (access(path.c_str(), R_OK) == -1)
            throw 403;

        page_content.open( path );
        if ( ! page_content.is_open() )
            throw 500 ;
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

/* in case large file eof = false and tranfer chunck by chunck */
void check_end_of_file(std::ifstream & page_content, bool & end_of_response, bool & tranfer_encoding )
{
    if ( page_content.eof())
        end_of_response = true;
    else
        tranfer_encoding = true;

    if ( tranfer_encoding && page_content.gcount() != 0 )
    {
        end_of_response = false;
    }
}


std::string extract_headers(std::string & unparsed_content)
{
    size_t pos;

    pos = unparsed_content.find("\r\n\r\n");
    if (pos == std::string::npos)
        throw 500;
    std::string headers = unparsed_content.substr(0, pos + 2);
    unparsed_content.erase(0, pos + 4);

    return ( headers );
}

std::string extract_body(const std::string & unparsed_content, size_t pos)
{
    return (unparsed_content.substr(pos, RESP_BUFF));
}


bool is_redirect(LocationContext * location)
{
    if (location && location->redirect_is_set)
        return true;
    return false;
}

std::string get_rand_file_name(size_t & file_num)
{
    std::ostringstream file_name;
    std::string path = "/tmp/.web_server_response_file_";

    file_name << path;
    file_name << file_num;

    if (access(file_name.str().c_str(), F_OK) == 0)
    {
        file_num += 1;
        return get_rand_file_name(file_num);
    }
    else
    {
        return file_name.str();
    }
}

void normalize_target(std::string &target, unsigned short & status)
{   
    if ( target.find("..") == std::string::npos || status != 200)
        return ;

    std::vector<std::string> directories = _split_(target, '/');
    std::vector<std::string>::iterator it = directories.begin();

    for (; it != directories.end(); it++)
    {
        if (*it == "." )
        {
            directories.erase(it);
            --it;
        }
    }

    it = directories.begin();
    while (it != directories.end())
    {
        if (it == directories.begin() && *it == "..")
        {
            status = 400;
            return ;
        }
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

static std::string is_image(const std::string & file_name)
{
    std::string imgs[] = {".png",".avif", ".gif", ".webp", ".dmp", ".apng"};
    size_t size = sizeof(imgs) / sizeof(imgs[0]);
    
    for (size_t i = 0; i < size; ++i)
    {
        if ( file_name.find(imgs[i], file_name.length() - imgs[i].length()) != std::string::npos )
            return ( "\r\nContent-Type: image/" +  imgs[i].erase(0, 1) );
    }
    return "";
}

static std::string is_text(const std::string & file_name)
{
    std::string texts[] = {".txt", ".html", ".htm", ".css", ".js"} ;
    size_t size = sizeof(texts) / sizeof(texts[0]);
    
    for (size_t i = 0; i < size ; ++i)
    {
        if ( file_name.find(texts[i], file_name.length() - texts[i].length()) != std::string::npos )
        {
            if ( texts[i] == ".js" || texts[i] == ".javascript")
                return ( "\r\nContent-Type: text/javascript; charset=utf-8") ;
            return ( "\r\nContent-Type: text/" +  texts[i].erase(0, 1) + "; charset=utf-8") ;
        }
    }
    return "";
}

static std::string is_image_jpeg(const std::string & file_name)
{
    std::string jpeg[] = {".jpg", ".jpeg", ".jfif", ".pjpeg", ".pjp"};
    size_t size = sizeof(jpeg) / sizeof(jpeg[0]);
    
    for ( size_t i = 0; i < size ; ++i)
    {
        if ( file_name.find(jpeg[i], file_name.length() - jpeg[i].length()) != std::string::npos)
            return ( "\r\nContent-Type: image/jpeg" ) ;
    }
    return "";
}

static std::string is_image_icon(const std::string & file_name)
{
    std::string ico[] = {".ico", ".cur"} ;
    size_t size = sizeof(ico) / sizeof(ico[0]);
    
    for (size_t i = 0; i < size; ++i)
    {
        if ( file_name.find(ico[i] ,file_name.length() - ico[i].length()) != std::string::npos )
            return ( "\r\nContent-Type: image/x-icon" );
    }
    return "";
}

static std::string is_video(const std::string & file_name)
{
    if ( file_name.find(".mp4", file_name.length() - 4) != std::string::npos )
        return ("\r\nContent-Type: video/mp4");
    return "";
}

static std::string is_audio(const std::string & file_name)
{
    if ( file_name.find(".mp3", file_name.length() - 4) != std::string::npos )
        return ("\r\nContent-Type: audio/mpeg");
    return "";
}

std::string get_content_type(const std::string & file_name)
{
    std::string type = "";

    type = is_text(file_name);
    if (type != "")
        return type;

    type = is_image(file_name);
    if (type != "")
        return type;

    type = is_image_jpeg(file_name);
    if (type != "")
        return type;

    type = is_image_icon(file_name);
    if (type != "")
        return type;
    
    type = is_video(file_name);
    if (type != "")
        return type;
    
    type = is_audio(file_name);
    if (type != "")
        return type;

    return ("\r\ntext/plain") ; /* text/plain or application/octet-stream depends on the server (nginx) configuration */
}

const std::string get_time(time_t sec)
{
    std::ostringstream res;
    struct  tm * time;

    time = localtime(&sec);

    res << (time->tm_year + 1900) << "-" ;
    if (time->tm_mon < 10)
        res << "0";
    res << (time->tm_mon + 1) << "-";

    if (time->tm_mday < 10)
        res << "0";
    res << time->tm_mday << " ";

    if (time->tm_hour < 10)
        res << "0";
    res << time->tm_hour << ":";

    if (time->tm_min < 10)
        res << "0";
    res << time->tm_min << ":";

    if (time->tm_sec < 10)
        res << "0";
    res << time->tm_sec;

    return res.str();
}

bool    is_dir( const std::string & path )
{
    struct stat s;

    if (stat(path.c_str(), &s) == -1)
        return (false);

    if (S_IFDIR & s.st_mode)
        return (true);

    return (false);
}

bool is_file(std::string path)
{
    struct  stat f;

    if (stat(path.c_str(), &f ) == -1)
        return false;

    if (f.st_mode & S_IFREG)
        return true;
        
    return false;
}

bool is_existe(const std::string & path)
{
    if ( access(path.c_str(), F_OK) == 0)
        return true;
        
    return false;
}

void check_is_cgi( Response & response )
{   
    unsigned short status = response.client_info.get_response()->get_status();
    if ((status >= 400 && status <= 599) || !response.server_context)
    {
        response.set_is_cgi(false);
        return ;
    }
    std::string path = response.get_path();
    size_t pos;
    pos = path.find_last_of(".");
    if (pos == std::string::npos)
    {
        response.set_is_cgi(false);
        return ;
    }

    extension extension_ = path.substr(pos);
    
    std::map<extension, execPath>::const_iterator end ;
    std::map<extension, execPath>::const_iterator pp ;
    
    if (response.get_location())
    {
        end = response.get_location()->get_cgi_info().end();
        pp = response.get_location()->get_cgi_info().find(extension_);
    }
    else
    {
        end = response.server_context->get_cgi_info().end();
        pp = response.server_context->get_cgi_info().find(extension_);
    }

    if (pp == end)
    {
        response.set_is_cgi(false);
        return ;
    }
    // pp = path position
    
    response.set_cgi_extention(pp->first);
    response.set_exec_path(pp->second);
    response.set_is_cgi(true);
}

void create_html_table(std::string & ls_files, const std::string & target)
{
    ls_files.append("<!DOCTYPE html><html lang=\"en\"><head><meta "
    "charset=\"UTF-8\"><title>" + target + "</title>"
    "<style>table{ padding-left: 100px;}td{ padding: 5px;}"
    "thead{ text-align: left;}</style></head><body><h1> Index of "
    + target + "</h1> <hr><table><thead><tr><th>Name</th>"
    "<th>Size</th><th>Date Modified</th></tr></thead><tbody>");
}

/* get info about file or dir and set it html table row & appent it in ls_files (body of response) */
void    append_row( const Response & response, struct dirent * f, std::string & ls_files )
{
    std::stringstream size;
    struct stat s;
    std::string path   = response.get_path();
    std::string target = response.get_target();
    std::string last_modification;
    

    if (path.back() != '/')
        path += ("/");

    if (target.back() != '/')
        target += ("/");
    path += (f->d_name);

    if ( stat(path.c_str(), &s ) == -1)
        return ;
    last_modification = get_time(s.st_mtime);    /* time of last modification */
    size << s.st_size;                          /* get the size of file and convert it to string (stringstream) */
    target += (f->d_name);                      /* concatinat target with / file name */
    
    /* set info to table row */
    ls_files += ("<tr><td><a href='" + target + "'>");
    ls_files += (f->d_name);
    ls_files += ("</a></td><td>" + size.str() + " bytes</td><td>");
    ls_files += last_modification;  
    ls_files += ("</td></tr>");
}

void    set_cgi_requerements( Response & response, bool & is_cgi)
{
    if ((response.get_location() && response.get_location() ->redirect_is_set))
        return ;

    check_is_cgi(response);
    if (is_cgi)
    {
        response.client_info.get_request()->is_cgi_request = true;

        response.set_input_path(get_rand_file_name(num_file));

        response.client_info.get_request()->cgi_content_file.open(response.get_input_path(), std::ios::out | std::ios::trunc | std::ios::binary);
        if (!response.client_info.get_request()->cgi_content_file.is_open())
        {
            is_cgi = false;
            response.set_status(500);
        }
    }
}


const ServerContext * get_server_context(ClientSocket & client_info)
{
    std::string host = client_info.get_request()->get_headers().find("HOST")->second ;

    for (std::vector<const ServerContext*>::const_iterator i = client_info.get_servers().begin() ; i != client_info.get_servers().end() ; ++i)
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
    return (client_info.get_servers().front()) ;
}

void extract_info_from_location(Response & response, LocationContext & location)
{
    if ((response.get_location() && response.get_location() ->redirect_is_set))
    {
        response.client_info.get_request()->upload_dir = location.get_upload_dir();
        return ;
    }

    std::string path = "";
    if (location.alias_is_set)
    {
        path = response.get_target();
        path.erase(0, location.get_location().length());
        path = location.get_alias() + path;
    }
    else 
    {
        path = location.get_root_directory() + response.get_target();
    }
    response.set_path(path);
    

    response.extract_pathinfo_form_target();

    response.set_upload_dir(location.get_upload_dir());
    response.client_info.get_request()->upload_dir = location.get_upload_dir();

     /* if the target is a directory. check if insid it a index . if index existe concatinat it with the path */
    if (is_dir(response.get_path()) && is_file(response.get_path() + "/" + location.get_index()))
        response.set_path(response.get_path() + "/" + location.get_index());
        
     /* if this path is a directory and autoindex off . that means error 403 (forbidden) */
    else if (is_dir(response.get_path()) && !location.get_auto_index() && response.client_info.get_request()->get_method() != "DELETE") 
        response.set_status(403);

    else if (!is_existe(response.get_path()))
        response.set_status(404);
}

void extract_info_from_server(Response & response,  const ServerContext & servercontext)
{
    response.set_path(servercontext.get_root_directory() + response.get_target());

    response.extract_pathinfo_form_target();
    
    response.set_upload_dir(servercontext.get_upload_dir());
    response.client_info.get_request()->upload_dir = servercontext.get_upload_dir();

     /* if the target is a directory. check if insid it a index . if index existe concatinat it with the path */
    if (is_dir(response.get_path()) && is_file(response.get_path() + "/" + servercontext.get_index()))
        response.set_path(response.get_path() + "/" + servercontext.get_index());
    
      /* if this path is a directory and autoindex off . that means error 403 (forbidden) */
    else if (is_dir(response.get_path()) && !servercontext.get_auto_index() && response.client_info.get_request()->get_method() != "DELETE")
        response.set_status(403);

    else if (!is_existe(response.get_path()))
        response.set_status(404);
}

void  remove_last_slash( std::string & target )
{
    size_t end = target.find_last_of('/') ;
    if ( end != std::string::npos )
    {
        if (target.back() == '/')
            target.pop_back();
        else
            target = target.substr(0, end + 1);
    }
    else
    {
        target = "";
    }
}

std::vector<std::string> split(const std::string & str)
{
    std::vector<std::string> strings;
    size_t i = 0 ;
    size_t start , end;
    
    while ( i < str.length() )
    {
        while ( i < str.length() && std::iswspace(str[i]) ) 
            i++ ;
        
        start = i ;
        
        while ( i < str.length() && ! std::iswspace(str[i]) ) 
            i++;

        end = i;

        if (start == end)
            break ;
        
        strings.push_back( str.substr( start, end - start ) );
    }
    return ( strings );
}

std::vector<std::string> _split_(const std::string & str, char c)
{
    std::vector<std::string> strings ;


    size_t i = 0 ;
    size_t start , end;
    
    while ( i < str.length() )
    {
        while ( i < str.length() && str[i] == c ) 
            i++ ;
        
        start = i ;
        while ( i < str.length() && str[i] != c ) 
            i++;

        end = i;
        if (start == end)
            break ;
        
        strings.push_back( str.substr( start, end - start ) );
    }
    return ( strings );
}

std::vector<std::string> split(const std::string & str, const std::string & to_find )
{
    std::vector<std::string> strings ;
    size_t start = 0;
    size_t pos = 0;

    while ( (pos = str.find(to_find, pos)) && pos != std::string::npos )
    {
        strings.push_back( str.substr( start,  pos - start) );
        pos += to_find.length();
        start = pos;
    }
    return ( strings );
}

void    print_log(Response & response, bool desconnected)
{
    if (!LOG_ENABLED)
        return ;

    time_t tm = time(0); 
    Request * request = response.client_info.get_request();
    
    std::string color = RESET;
    if (response.get_status() >= 400 && response.get_status()  <= 599)
        color = RED;
    else if (response.get_status() >= 300 && response.get_status()  <= 399)
        color = CYAN;
        
    
    std::cout   << color
                << "[ " 
                << get_time(tm) << " ] \"" 
                << request->get_method() 
                << " " 
                << request->get_target()  
                << " " << request->get_version() 
                << "\" "  << response.get_status() 
                << " " << response.body_size
                << RESET
                << std::endl;

    if (desconnected)
        std::cout   << CYAN
                << "[ " 
                << get_time(tm)
                << " ] \"Client disconected!"
                << RESET
                << std::endl;
    
    
}
