/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpContext.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:01:34 by ymafaman          #+#    #+#             */
/*   Updated: 2024/10/15 10:26:00 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpContext.hpp"

HttpContext::HttpContext( void )
{
    this->auto_index = false;
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

void    HttpContext::show_info()
{
    std::cout << "Major HTTP context info :" << std::endl;
    std::cout << "Number of defined error pages : " << this->error_pages.size() << std::endl;
    std::cout << "Error pages info : " << std::endl;

    for (std::vector<std::pair <unsigned short, std::string> >::iterator it = error_pages.begin(); it < error_pages.end(); it++)
    {
        std::cout << "Status code : " << "\"" << it->first << "\"" << " Path : " << "\"" << it->second << "\"" << std::endl;
    }
    
    std::cout << "Defined CGI extension : " << "\"" << cgi_extension << "\"" << std::endl;

    std::cout << "Auto index state : ";
    if (auto_index)
        std::cout << "\"ON\"" << std::endl;
    else
        std::cout << "\"OFF\"" << std::endl;

    std::cout << "Number of defined server contexts : " << this->servers.size() << std::endl;
    std::cout << "Servers info : " << std::endl;
    
    for (std::vector<ServerContext>::iterator it = servers.begin(); it < servers.end(); it++)
    {
        it->show_info();
        std::cout << "      " << std::endl << "------------------------------------" << std::endl;
    }
}