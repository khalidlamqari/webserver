/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationContext.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:31:25 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/03 18:08:39 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_CONTEXT_HPP
# define LOCATION_CONTEXT_HPP

#include <vector>
#include <string>
#include <iostream>


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

    /* LocationContext Destructor */
    ~LocationContext();

    /* Setters */

    void    set_error_page      ( const std::pair<unsigned short, std::string>& error_info );
    void    set_redirection     ( std::pair<unsigned short, std::string> redirection_info );
    void    set_root_directory  ( std::string root );
    void    set_cgi_extension   ( const std::string& extension );
    void    set_upload_dir      ( std::string directory );
    void    set_index           ( std::string index );
    void    set_auto_index      ( bool on_off );
    void    set_allowed_methods ( std::vector<std::string> methods );
    void    make_exact          ( void );
    void    set_location        ( std::string location );

    /* Getters */

    std::vector<std::pair <unsigned short, std::string> > &              get_error_pages      ( void ) ;
    const std::pair<unsigned short, std::string>&              get_redirection     ( void ) const ;
    const std::string&                                         get_root_directory  ( void ) const ;
    const std::string&                                         get_cgi_extension   ( void ) const ;
    const std::string&                                         get_upload_dir      ( void ) const ;
    const std::string&                                         get_index           ( void ) const ;
    bool                                                       get_auto_index      ( void ) const ;
    const std::vector<std::string>&                            get_allowed_methods ( void ) const ;
    const std::string&                                         get_location        ( void ) const ;
    bool                                                       is_exact_location   ( void ) const ;
    
    //Testing 
    void    show_info();

    private :

        std::vector<std::pair <unsigned short, std::string> >   error_pages;

        std::pair <unsigned short, std::string>                 redirection;

        std::vector<std::string>                                allowed_methods;  

        std::string                                             location; // TODO

        std::string                                             root_directory;

        std::string                                             cgi_extension;

        std::string                                             upload_dir ;

        std::string                                             index ;

        bool                                                    auto_index ;

        bool                                                    is_exact; // TODO
};


#endif
