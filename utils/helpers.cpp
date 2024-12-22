/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 14:16:59 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/22 10:43:05 by klamqari         ###   ########.fr       */
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


void    set_connection(const std::map<std::string , std::string> & headers, std::string & connection )
{
    std::map<std::string, std::string>::const_iterator it = headers.find("CONNECTION");
    if ( it == headers.end() )
    {
        connection = "keep-alive";
    }
    else if ( it->second != "close")
    {
        connection = "keep-alive";
    }
    else
    {
        connection = it->second;
    }
}

bool is_existe(const std::string & path)
{
    if ( access(path.c_str(), F_OK) == 0)
        return true;
    return false;
}

bool check_is_cgi(std::string & path, std::string & cgi_extention, bool is_bad_request)
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