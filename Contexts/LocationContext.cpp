/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationContext.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/12 20:30:49 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/18 11:44:14 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationContext.hpp"

LocationContext::LocationContext( const std::string & location ) : location(location)
{
    redirection.status_code = 0; // to be able to determine if a redirection was provided or not

    root_is_set = false;
    index_is_set = false;
    cgi_ext_is_set = false;
    methods_is_set = false;
    upl_dir_is_set = false;
    auto_ind_is_set = false;
    redirect_is_set = false;
}

LocationContext::LocationContext( void )
{
    redirection.status_code = 0; // to be able to determine if a redirection was provided or not

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

void    LocationContext::set_error_page(const t_error_page & error_info)
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

void    LocationContext::set_auto_index( const std::string & on_off )
{
    if (on_off == "on")
        this->auto_index = true;
    else
        this->auto_index = false;
}

void    LocationContext::set_allowed_methods( std::vector<std::string> methods )
{
    this->allowed_methods.assign(methods.begin(), methods.end());
}

void    LocationContext::set_redirection( t_redirection_info redirection_info )
{
    this->redirection = redirection_info;
}

void    LocationContext::set_location( std::string location )
{
    this->location = location;
}

/* Getters */

const std::vector<t_error_page> &              LocationContext::get_error_pages      ( void ) const
{
    return ( this->error_pages ) ;
}

const t_redirection_info &   LocationContext::get_redirection     ( void ) const
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

const bool &  LocationContext::get_auto_index      ( void ) const
{
    return ( this->auto_index ) ;    
}

const std::vector<std::string>& LocationContext::get_allowed_methods ( void )  const
{
    return ( this->allowed_methods ) ;    
}

const std::string&  LocationContext::get_location        ( void ) const
{
    return ( this->location ) ;
}

// bool  LocationContext::is_exact_location        ( void ) const
// {
//     return ( this->is_exact ) ;
// }