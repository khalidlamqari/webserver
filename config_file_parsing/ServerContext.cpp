/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContext.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/12 16:37:22 by ymafaman          #+#    #+#             */
/*   Updated: 2024/10/31 17:03:56 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerContext.hpp"

ServerContext::ServerContext( void )
{
    this->index = "index.html"; // Setting "index.html" as default index.

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

void    ServerContext::set_error_page(const std::pair <unsigned short, std::string>& error_info)
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

void   ServerContext::set_new_location( void )
{
    LocationContext   new_location;

    std::vector<std::pair <unsigned short, std::string> >::iterator it;

    /* Making the new location inherit the serverContext directives. */
    for (it = this->error_pages.begin(); it < this->error_pages.end(); it++)
        new_location.set_error_page(*it);

    new_location.set_allowed_methods(this->allowed_methods);
    new_location.set_root_directory(this->root_directory);
    new_location.set_cgi_extension(this->cgi_extension);
    new_location.set_upload_dir(this->upload_dir);
    new_location.set_index(this->index);
    new_location.set_auto_index(this->auto_index);

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

void    ServerContext::set_auto_index(bool on_off)
{
    if (on_off)
        this->auto_index = true;
    else
        this->auto_index = false;
}

void    ServerContext::set_allowed_methods( std::vector<std::string> methods )
{
    this->allowed_methods.assign(methods.begin(), methods.end());
}

LocationContext&  ServerContext::get_latest_location( void )
{
    return this->locations.back();
}

void ServerContext::show_info()
{
    std::cout << "      " << "Server names : ";

    for (std::vector<std::string>::iterator it = server_names.begin(); it < server_names.end(); it++)
    {
        std::cout << "      " << "\"" << *it << "\""  << " ";
    }
    std::cout << std::endl;
    
    std::cout << "      " << "Port numbers : ";

    for (std::vector<unsigned short>::iterator it = ports.begin(); it < ports.end(); it++)
            std::cout << "\"" << *it << "\" ";

    std::cout << std::endl;

    std::cout << "      " << "Root directory : " << "\"" << root_directory << "\"" << std::endl;

    std::cout << "      " << "Allowed methods : ";
    

    for (std::vector<std::string>::iterator it = allowed_methods.begin(); it < allowed_methods.end(); it++)
    {
        std::cout << "      " << "\"" << *it << "\"" << " ";
    }
    std::cout << std::endl;

    std::cout << "      " << "Index : " << "\"" << index << "\"" << std::endl;

    std::cout << "      " << "Upload directory : " << "\"" << upload_dir << "\"" << std::endl;

    std::cout << "      " << "Number of defined error pages : " << this->error_pages.size() << std::endl;
    std::cout << "      " << "Error pages info : " << std::endl;

    for (std::vector<std::pair <unsigned short, std::string> >::iterator it = error_pages.begin(); it < error_pages.end(); it++)
    {
        std::cout << "      " << "Status code : " << "\"" << it->first << "\"" << " Path : " << "\"" << it->second << "\"" << std::endl;
    }
    
    std::cout << "      " << "Defined CGI extension : " << "\"" << cgi_extension << "\"" << std::endl;

    std::cout << "      " << "Auto index state : ";
    if (auto_index)
        std::cout << "      " << "\"ON\"" << std::endl;
    else
        std::cout << "      " << "\"OFF\"" << std::endl;

    std::cout << "      " << "Number of defined location contexts : " << this->locations.size() << std::endl;
    std::cout << "      " << "Locations info : " << std::endl;
    
    for (std::vector<LocationContext>::iterator it = locations.begin(); it < locations.end(); it++)
    {
        it->show_info();
    }
}


/* Getters */
const std::vector<std::pair <unsigned short, std::string> > & ServerContext::get_error_page      ( void )
{
    return ( this->error_pages );
}

std::vector<LocationContext>  & ServerContext::get_locations ( void )
{
    return ( this->locations ) ;
}

 std::vector<unsigned short>   & ServerContext::get_ports ( void )
{
    return ( this->ports );
}

const std::string   & ServerContext::get_root_directory  ( void )
{
    return ( this->root_directory );
}

const std::string & ServerContext::get_cgi_extension   ( void )
{
    return ( this->cgi_extension ) ;
}

const std::string & ServerContext::get_upload_dir      ( void )
{
    return ( this->upload_dir );
}

const std::string & ServerContext::get_index           ( void )
{
    return ( this->index ) ;
}

std::vector<std::string> & ServerContext::get_server_names    ( void )
{
    return ( this->server_names ) ;
}

bool ServerContext::get_auto_index( void )
{
    return ( this->auto_index ) ;
}

std::vector<std::string> & ServerContext::get_allowed_methods ( void )
{
    return ( this->allowed_methods ) ;
}