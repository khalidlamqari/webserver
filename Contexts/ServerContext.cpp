/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContext.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/12 16:37:22 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/30 05:32:28 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerContext.hpp"

ServerContext::ServerContext( void )
{
    this->index = "index.html"; // Setting "index.html" as default index.

    host_is_set = false;
    root_is_set = false;
    port_is_set = false;
    index_is_set = false;
    cgi_ext_is_set = false;
    methods_is_set = false;
    upl_dir_is_set = false;
    auto_ind_is_set = false;
    srv_names_is_set = false;
}

ServerContext::~ServerContext()
{
    
}

void    ServerContext::set_error_page(const t_error_page& error_info)
{
    std::vector<t_error_page>::iterator it;

    for (it = this->error_pages.begin(); it != this->error_pages.end(); it++)
    {
        if ((*it).err_code == error_info.err_code)
        {
            (*it).path = error_info.path;
            return ;
        }
    }

    this->error_pages.push_back(error_info);
}

void   ServerContext::set_new_location( const std::string & location )
{
    LocationContext   new_location(location);

    std::vector<t_error_page>::iterator it;

    /* Making the new location inherit the serverContext directives. */
    for (it = this->error_pages.begin(); it < this->error_pages.end(); it++)
        new_location.set_error_page(*it);

    new_location.set_allowed_methods(this->allowed_methods);
    new_location.set_root_directory(this->root_directory);
    new_location.set_cgi_extension(this->cgi_extension);
    new_location.set_upload_dir(this->upload_dir);
    new_location.set_index(this->index);

    if (this->auto_index)
        new_location.set_auto_index("on");
    else
        new_location.set_auto_index("off");

    this->locations.push_back(new_location);
}

void    ServerContext::set_ports( std::vector<unsigned short> ports )
{
    this->ports.assign(ports.begin(), ports.end());
}

void    ServerContext::set_root_directory( std::string root )
{
    this->root_directory = root;
}

void    ServerContext::set_cgi_extension(const std::string& extension)
{
    this->cgi_extension = extension;
}

void    ServerContext::set_upload_dir( std::string directory )
{
    this->upload_dir = directory;
}

void    ServerContext::set_index( std::string index )
{
    this->index = index;
}

void    ServerContext::set_server_names( std::vector<std::string> names )
{
    this->server_names.assign(names.begin(), names.end());
}

void    ServerContext::set_auto_index(const std::string & on_off)
{
    if (on_off == "on")
        this->auto_index = true;
    else
        this->auto_index = false;
}

void    ServerContext::set_allowed_methods( std::vector<std::string> methods )
{
    this->allowed_methods.assign(methods.begin(), methods.end());
}

void    ServerContext::set_host( std::string host )
{
    this->host = host;
}


LocationContext&  ServerContext::get_latest_location( void )
{
    return this->locations.back();
}


/* Getters */

const std::vector<t_error_page>& ServerContext::get_error_pages( void ) const
{
    return this->error_pages;
}

const std::vector<LocationContext>& ServerContext::get_locations( void ) const
{
    return this->locations;
}

const std::vector<std::string>& ServerContext::get_server_names( void ) const
{
    return this->server_names;
}

const std::vector<std::string>& ServerContext::get_allowed_methods( void ) const
{
    return this->allowed_methods;
}

const std::vector<unsigned short>& ServerContext::get_ports( void ) const
{
    return this->ports;
}

const std::string& ServerContext::get_root_directory( void ) const
{
    return this->root_directory;
}

const std::string& ServerContext::get_cgi_extension( void ) const
{
    return this->cgi_extension;
}

const std::string& ServerContext::get_upload_dir( void ) const
{
    return this->upload_dir;
}

const std::string& ServerContext::get_index( void ) const
{
    return this->index;
}

const std::string& ServerContext::get_host( void ) const
{
    return this->host;
}

const bool& ServerContext::get_auto_index( void ) const
{
    return this->auto_index;
}
