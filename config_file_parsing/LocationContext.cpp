/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationContext.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/12 20:30:49 by ymafaman          #+#    #+#             */
/*   Updated: 2024/10/15 10:27:03 by ymafaman         ###   ########.fr       */
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

void    LocationContext::show_info()
{

    std::cout << "              " << "Location : " << "\"" << location << "\"";

    if (is_exact)
        std::cout << "(exact)" << std::endl;
    else
        std::cout << std::endl;

    std::cout << "              " << "Root directory : " << "\"" << root_directory << "\"" << std::endl;

    std::cout << "              " << "Allowed methods : ";
    
    for (std::vector<std::string>::iterator it = allowed_methods.begin(); it < allowed_methods.end(); it++)
    {
        std::cout << "              " << "\"" << *it << "\"" << " ";
    }
    std::cout << std::endl;

    std::cout << "              " << "Index : " << "\"" << index << "\"" << std::endl;

    std::cout << "              " << "Upload directory : " << "\"" << upload_dir << "\"" << std::endl;

    std::cout << "              " << "Number of defined error pages : " << this->error_pages.size() << std::endl;
    std::cout << "              " << "Error pages info : " << std::endl;

    for (std::vector<std::pair <unsigned short, std::string> >::iterator it = error_pages.begin(); it < error_pages.end(); it++)
    {
        std::cout << "              " << "Status code : " << "\"" << it->first << "\"" << " Path : " << "\"" << it->second << "\"" << std::endl;
    }
    
    std::cout << "              " << "Defined CGI extension : " << "\"" << cgi_extension << "\"" << std::endl;

    std::cout << "              " << "Auto index state : ";
    if (auto_index)
        std::cout << "              " << "\"ON\"" << std::endl;
    else
        std::cout << "              " << "\"OFF\"" << std::endl;
    
    if (redirection.first != 0)
        std::cout << "              " << "Redirection tatus code: " << "\"" << redirection.first << "\"" << " | Redirection Path/Message : " << "\"" << redirection.second << "\"" << std::endl;
    else
        std::cout << "No redirections provided." << std::endl;
}

void    LocationContext::set_location( std::string location )
{
    this->location = location;
}