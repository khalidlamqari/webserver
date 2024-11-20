/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpContext.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:01:34 by ymafaman          #+#    #+#             */
/*   Updated: 2024/10/30 22:38:00 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpContext.hpp"

HttpContext::HttpContext( void )
{
    this->auto_index = false;
    this->max_body_size = 1000000;
}

HttpContext::~HttpContext()
{
    
}

void    HttpContext::set_error_page(const std::pair <unsigned short, std::string>& error_info)
{
    std::vector<std::pair <unsigned short, std::string> >::iterator it;

    for (it = this->error_pages.begin(); it != this->error_pages.end(); it++)
    {
        if ((*it).first == error_info.first)
        {
            (*it).second = error_info.second;

            return ;
        }
    }

    this->error_pages.push_back(error_info);
}

void    HttpContext::set_auto_index(bool on_off)
{
    if (on_off)
        this->auto_index = true;
    else
        this->auto_index = false;
}

void    HttpContext::set_cgi_extension(const std::string& extension)
{
    this->cgi_extension = extension;
}

void   HttpContext::set_new_server( void )
{
    ServerContext   new_server;

    std::vector<std::pair <unsigned short, std::string> >::iterator it;

    /* Making the new server inherit the httpContext directives. */
    for (it = this->error_pages.begin(); it < this->error_pages.end(); it++)
        new_server.set_error_page(*it);

    new_server.set_cgi_extension(this->cgi_extension);
    new_server.set_auto_index(this->auto_index);
    
    /* Pushing the newly created server to the servers vector */
    this->servers.push_back(new_server);
}

void    HttpContext::set_max_body_size( size_t limit )
{
    this->max_body_size = limit;
}


ServerContext&  HttpContext::get_latest_server( void )
{
    return this->servers.back();
}

const std::vector<ServerContext>& HttpContext::get_servers( void ) const
{
    return this->servers;
}

const std::vector<std::pair <unsigned short, std::string> >& HttpContext::get_error_pages( void ) const
{
    return this->error_pages;
}

const std::string&  HttpContext::get_cgi_extension( void ) const
{
    return this->cgi_extension;
}

const bool& HttpContext::get_auto_index( void ) const
{
    return this->auto_index;
}
