/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContext.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:31:30 by ymafaman          #+#    #+#             */
/*   Updated: 2024/10/30 22:40:05 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONTEXT_HPP
# define SERVER_CONTEXT_HPP

#include <vector>
#include <string>
#include <iostream>

#include "LocationContext.hpp"

class ServerContext {

    public :
        
        /*  Flags to detect duplications in configuration file. */
        
        bool     srv_names_is_set;
        bool     auto_ind_is_set;
        bool     cgi_ext_is_set;
        bool     methods_is_set;
        bool     upl_dir_is_set;
        bool     index_is_set;
        bool     port_is_set;
        bool     root_is_set;
        bool     host_is_set;

        /* ServerContext Constructor */
        ServerContext( void );

        /* ServerContext Destructor */
        ~ServerContext();

        /* Setters */
        void    set_error_page      ( const std::pair <unsigned short, std::string>& error_info );
        void    set_new_location    ( void );
        void    set_ports           ( std::vector<unsigned short> ports );
        void    set_root_directory  ( std::string root );
        void    set_cgi_extension   ( const std::string& extension );
        void    set_upload_dir      ( std::string directory );
        void    set_index           ( std::string index );
        void    set_server_names    ( std::vector<std::string> names );
        void    set_auto_index      ( bool on_off );
        void    set_allowed_methods ( std::vector<std::string> methods );
        void    set_host            ( std::string host );

        /* The following method returns the latest inserted locationContext in the locations vector in order to store it's info. */
        LocationContext&  get_latest_location( void );

        /* Getters */

        const std::vector<std::pair <unsigned short, std::string> >&    get_error_pages( void ) const;
        const std::vector<LocationContext>&                             get_locations( void ) const;
        const std::vector<std::string>&                                 get_server_names( void ) const;
        const std::vector<std::string>&                                 get_allowed_methods( void ) const;
        const std::vector<unsigned short>&                              get_ports( void ) const;
        const std::string&                                              get_root_directory( void ) const;
        const std::string&                                              get_cgi_extension( void ) const;
        const std::string&                                              get_upload_dir( void ) const;
        const std::string&                                              get_index( void ) const;
        const std::string&                                              get_host( void ) const;
        const bool&                                                     get_auto_index( void ) const;  

    private :

        std::vector<std::pair <unsigned short, std::string> >   error_pages; // better make a struct that encaplsulates the error page info.

        std::vector<LocationContext>                            locations; // TODO : Is making a separated vector for exact locations better that mixing all locations in one single vector. 

        std::vector<std::string>                                server_names;

        std::vector<std::string>                                allowed_methods;

        std::vector<unsigned short>                             ports;

        std::string                                             root_directory;

        std::string                                             cgi_extension;

        std::string                                             upload_dir;

        std::string                                             index;

        std::string                                             host;

        bool                                                    auto_index;

};



#endif


