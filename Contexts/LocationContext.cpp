/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationContext.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/12 20:30:49 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/20 03:51:31 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationContext.hpp"

LocationContext::LocationContext( void )
{
    this->is_exact = false;
    redirection.first = 0; // to be able to determine if a redirection was provided or not

    root_is_set = false;
    index_is_set = false;
    cgi_ext_is_set = false;
    methods_is_set = false;
    upl_dir_is_set = false;
    auto_ind_is_set = false;
    redirect_is_set = false;
}

LocationContext::~LocationContext()
{
    
}

void    LocationContext::set_error_page(const std::pair <unsigned short, std::string>& error_info)
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

void    LocationContext::set_root_directory( std::string root )
{
    this->root_directory = root;
}

void    LocationContext::set_cgi_extension(const std::string& extension)
{
    this->cgi_extension = extension;
}

void    LocationContext::set_upload_dir( std::string directory )
{
    this->upload_dir = directory;
}

void    LocationContext::set_index( std::string index )
{
    this->index = index;
}

void    LocationContext::set_auto_index(bool on_off)
{
    if (on_off)
        this->auto_index = true;
    else
        this->auto_index = false;
}

void    LocationContext::set_allowed_methods( std::vector<std::string> methods )
{
    this->allowed_methods.assign(methods.begin(), methods.end());
}

void    LocationContext::make_exact( void )
{
    this->is_exact = true;
}

void    LocationContext::set_redirection( std::pair<unsigned short, std::string> redirection_info )
{
    this->redirection = redirection_info;
}

void    LocationContext::set_location( std::string location )
{
    this->location = location;
}


/* Getters */

std::vector<std::pair <unsigned short, std::string> > &              LocationContext::get_error_pages      ( void )
{
    return ( this->error_pages ) ;
}

const std::pair<unsigned short, std::string>&   LocationContext::get_redirection     ( void ) const
{
    return ( this->redirection ) ;    
}

const std::string&  LocationContext::get_root_directory  ( void ) const
{
    return ( this->root_directory ) ;    
}

const std::string&  LocationContext::get_cgi_extension   ( void ) const
{
    return ( this->cgi_extension ) ;    
}

const std::string&  LocationContext::get_upload_dir      ( void ) const
{
    return ( this->upload_dir ) ;    
}

const std::string&  LocationContext::get_index           ( void ) const
{
    return ( this->index ) ;    
}

bool   LocationContext::get_auto_index      ( void ) const
{
    return ( this->auto_index ) ;    
}

std::vector<std::string>& LocationContext::get_allowed_methods ( void )  
{
    return ( this->allowed_methods ) ;    
}

const std::string&  LocationContext::get_location        ( void ) const
{
    return ( this->location ) ;
}

bool  LocationContext::is_exact_location        ( void ) const
{
    return ( this->is_exact ) ;
}