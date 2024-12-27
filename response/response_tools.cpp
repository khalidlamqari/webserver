/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_tools.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 08:55:51 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/27 18:00:35 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/main.hpp"
# include "DefaultInfo.hpp"

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
        throw 500;

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

void    set_connection_header(std::string & message, bool close, unsigned short & status)
{
    if ( close || (status >= 400 && status <= 599))
        message += ("Connection: close\r\n");
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

/* in case large file eof = false and tranfer chunck by chunck */
void check_end_of_file(std::ifstream & page_content, bool & end_of_response, bool & tranfer_encoding )
{
    if ( page_content.eof())
        end_of_response = true;
    else
        tranfer_encoding = true;

    if ( tranfer_encoding && page_content.gcount() != 0 )
        end_of_response = false;
}


std::string extract_headers(const std::string & unparsed_content, size_t pos)
{
    return ( unparsed_content.substr(0, pos + 4) );
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
    if ( target.find("..") == std::string::npos )
        return ;
    std::vector<std::string> directories = _split_(target, '/');
    std::vector<std::string>::iterator it = directories.begin();
    while (it != directories.end())
    {
        if (it == directories.begin() && *it == "..")
        {   
            status = 403;
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
    std::string imgs[] = {".png",".avif", ".gif", ".webp", ".dmp", ".apng"} ;

    for (size_t i = 0; i < imgs->length(); ++i)
    {
        if ( file_name.find(imgs[i], file_name.length() - imgs[i].length()) != std::string::npos )
            return ( "\r\nContent-Type: image/" +  imgs[i].erase(0, 1) ) ;
    }
    return "";
}

static std::string is_text(const std::string & file_name)
{
    std::string texts[] = {".txt", ".html", ".htm", ".css", ".js"} ;
    
    for (size_t i = 0; i < texts->length() ; ++i)
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

    for ( size_t i = 0; i < jpeg->length() ; ++i)
    {
        if ( file_name.find(jpeg[i], file_name.length() - jpeg[i].length()) != std::string::npos)
            return ( "\r\nContent-Type: image/jpeg" ) ;
    }
    return "";
}

static std::string is_image_icon(const std::string & file_name)
{
    std::string ico[] = {".ico", ".cur"} ;

    for (size_t i = 0; i < ico->length(); ++i)
    {
        if ( file_name.find(ico[i] ,file_name.length() - ico[i].length()) != std::string::npos )
            return ( "\r\nContent-Type: image/x-icon" ) ;
    }
    return "";
}

static std::string is_video(const std::string & file_name)
{
    if ( file_name.find(".mp4", file_name.length() - 4) != std::string::npos )
        return ( "\r\nContent-Type: video/mp4" ) ;
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

    return ("") ;
}

/* January 1, 1970, 00:00:00 */
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
    struct stat s ;

    if ( stat(path.c_str(), &s) == -1)
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

bool check_is_cgi(const std::string & path, const std::string & cgi_extention, bool is_bad_request) 
{
    return (!is_bad_request && !cgi_extention.empty()
        && (path.length() - cgi_extention.length() > 0)
        && (path.find(cgi_extention, path.length()
        - cgi_extention.length()) != std::string::npos));
}

void create_socket_pair(Response & response)
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, response.get_pair_fds() ) == -1)
    {
        throw std::runtime_error("socketpair failed");
    }
    response.set_input_path(get_rand_file_name(num_file));
}

void create_html_table(std::string & ls_files, const std::string & target)
{
    ls_files.append("<!DOCTYPE html><html lang=\"en\"><head><meta "
    "charset=\"UTF-8\"><title>" + target + "</title>"
    "<style>table{ padding-left: 100px;}td{ padding: 5px;}"
    "thead{ text-align: left;}</style></head><body><h1> Index of "
    + target + "</h1> <hr><table><thead><tr><th>Name</th>"
    "<th>Size</th><th>Date Modified</th></tr></thead><tbody>") ;
}

/* get info about file or dir and set it html table row & appent it in ls_files (body of response) */
void    append_row( std::string  path , std::string target, struct dirent * f, std::string & ls_files )
{
    std::stringstream size ;
    struct stat s;

    if (path.back() != '/')
        path += ("/");
        
    if (target.back() != '/')
        target += ("/");

    path += (f->d_name);

    if ( stat(path.c_str(), &s ) == -1)
        throw 404;

    size << f->d_reclen;        /* get the size of file and convert it to string (stringstream) */
    target += (f->d_name);     /* concatinat target with / file name */

    /* set info to table row */
    ls_files += ("<tr><td><a href='" + target + "'>");
    ls_files += (f->d_name);
    ls_files += ("</a></td><td>" + size.str() + " bytes</td><td>");
    ls_files += (get_time(s.st_mtime));   /* time of last modification */
    ls_files += ("</td></tr>");
}

void    set_cgi_requerements( Response & response, bool & is_cgi)
{
    if (check_is_cgi(response.get_path(), response.get_cgi_exrention() , response.clientsocket.get_request()->isBadRequest() )) // , 
    {
        is_cgi = true;
    }

    if (is_cgi)
        create_socket_pair(response);
    
}


const ServerContext * get_server_context(ClientSocket & clientsocket)
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

void extract_info_from_location(Response & response, LocationContext & location)
{
    response.set_cgi_extention(location.get_cgi_extension());
    response.extract_pathinfo_form_target(location.get_root_directory());
    
    response.set_upload_dir(location.get_upload_dir());
    response.set_path(location.get_root_directory() + response.get_target());
    
    if (is_dir(response.get_path()) && is_file(response.get_path() + "/" + location.get_index())) /* if the target is a directory. check if insid it a index . if index existe concatinat it with the path */
        response.set_path(response.get_path() + "/" + location.get_index());
        
    else if (is_dir(response.get_path()) && !location.get_auto_index() && response.clientsocket.get_request()->get_method() != "DELETE")  /* if this path is a directory and autoindex off . that means error 403 (forbidden) */
        response.set_status(403);

    else if (!is_existe(response.get_path()))
        response.set_status(404);
}

void extract_info_from_server(Response & response,  const ServerContext & servercontext)
{
    response.set_cgi_extention(servercontext.get_cgi_extension());
    response.extract_pathinfo_form_target(servercontext.get_root_directory());
    
    response.set_upload_dir(servercontext.get_upload_dir());
    response.set_path(servercontext.get_root_directory() + response.get_target());

    if (is_dir(response.get_path()) && is_file(response.get_path() + "/" + servercontext.get_index())) /* if the target is a directory. check if insid it a index . if index existe concatinat it with the path */
        response.set_path(response.get_path() + "/" + servercontext.get_index());

    else if (is_dir(response.get_path()) && !servercontext.get_auto_index() && response.clientsocket.get_request()->get_method() != "DELETE")  /* if this path is a directory and autoindex off . that means error 403 (forbidden) */
        response.set_status(403);

    else if (!is_existe(response.get_path()))
        response.set_status(404);
}

void  remove_last_slash( std::string & target )
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

std::vector<std::string> split(const std::string & str)
{
    std::vector<std::string> strings ;


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