/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationContext.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:31:25 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/18 11:04:13 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_CONTEXT_HPP
# define LOCATION_CONTEXT_HPP

#include <vector>
#include <string>
#include <iostream>

#include "../config_file_parsing/config_structs.hpp"

class LocationContext {

    public :

    /*  Flags to detect duplications in configuration file. */

    bool     redirect_is_set;
    bool     auto_ind_is_set;
    bool     cgi_ext_is_set;
    bool     methods_is_set;
    bool     upl_dir_is_set;
    bool     index_is_set;
    bool     root_is_set;

    /* LocationContext Constructor */
    LocationContext( void );
    LocationContext( const std::string & location );

    /* LocationContext Destructor */
    ~LocationContext();

    /* Setters */

    void    set_error_page      ( const t_error_page & error_info );
    void    set_redirection     ( t_redirection_info redirection_info );
    void    set_root_directory  ( std::string root );
    void    set_cgi_extension   ( const std::string& extension );
    void    set_upload_dir      ( std::string directory );
    void    set_index           ( std::string index );
    void    set_auto_index      ( const std::string & on_off );
    void    set_allowed_methods ( std::vector<std::string> methods );
    void    set_location        ( std::string location );

        /* Getters */

    const std::vector<t_error_page> &                          get_error_pages      ( void ) const;
    const t_redirection_info &                                 get_redirection     ( void ) const;
    const std::string&                                         get_root_directory  ( void ) const ;
    const std::string&                                         get_cgi_extension   ( void ) const ;
    const std::string&                                         get_upload_dir      ( void ) const ;
    const std::string&                                         get_index           ( void ) const ;
    const bool &                                               get_auto_index      ( void ) const ;
    const std::vector<std::string>&                            get_allowed_methods ( void ) const ;
    const std::string&                                         get_location        ( void ) const ;
    // bool                                                       is_exact_location   ( void ) const ;

    private :

        std::vector<t_error_page>                               error_pages;

        t_redirection_info                                      redirection;

        std::vector<std::string>                                allowed_methods;

        std::string                                             location;

        std::string                                             root_directory;

        std::string                                             cgi_extension;

        std::string                                             upload_dir;

        std::string                                             index;

        bool                                                    auto_index;

};


#endif