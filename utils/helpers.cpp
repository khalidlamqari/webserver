/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 14:16:59 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/23 12:51:07 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/main.hpp"
# include <sys/stat.h>
#include "../response/Response.hpp"

static size_t num_file = 0;

bool is_all_WS( const std::string & str )
{
    for ( size_t i = 0; i < str.length() ; i++ )
        if ( ! std::iswspace(str[i]) )
            return ( false ) ;
    return ( true ) ;
}

bool is_all_digit(const std::string & str)
{
    for ( size_t i = 0; i < str.length() ; i++ )
        if ( ! std::isdigit( str[i]) )
            return ( false ) ;
    return ( true ) ;
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

void normalize_target(std::string &target, short & status)
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
            {
                --it;
            }
        }
        else
        {   
            ++it;
        }
    }
    target = "/";
    for (std::vector<std::string>::iterator it = directories.begin() ; it != directories.end() ; ++it) 
        target += "/" + *it;
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
    "charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width"
    ", initial-scale=1.0\"><title>" + target + "</title>"
    "<style>table{ padding-left: 100px;}td{ padding: 5px;}"
    "thead{ text-align: left;}</style></head><body><h1> Index of "
    + target + "</h1> <hr><table><thead><tr><th>Name</th>"
    "<th>Size</th><th>Date Modified</th></tr></thead><tbody>") ;
}

void    set_headers(std::string & message, const std::string & content_len)
{
    message += ("HTTP/1.1 200 OK\r\nContent-Length: "\
            + content_len \
            + "\r\nContent-Type: text/html\r\n\r\n");
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

void    set_cgi_requerements( Response & response, bool & is_cgi , std::ofstream & input_data)
{
    if (check_is_cgi(response.get_path(), response.get_cgi_exrention() , response.clientsocket.get_request()->isBadRequest() )) // , 
    {
        is_cgi = true;
    }

    if (is_cgi)
    {
        create_socket_pair(response);
        input_data.open(response.get_input_path());
        if (!input_data.is_open())
        {
            throw std::runtime_error("open failed");
        }
    }
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
        
    else if (is_dir(response.get_path()) && !location.get_auto_index())  /* if this path is a directory and autoindex off . that means error 403 (forbidden) */
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
    else if (is_dir(response.get_path()) && !servercontext.get_auto_index())  /* if this path is a directory and autoindex off . that means error 403 (forbidden) */
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
