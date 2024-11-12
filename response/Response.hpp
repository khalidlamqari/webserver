/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:57 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/12 03:23:32 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../includes/main.hpp"
# include "ErrorPages.hpp"
# include <sys/stat.h>
# include <dirent.h>
#include <time.h>
#include <fstream>

class Request ;
 

static ErrorPages default_error_pages ;

class Response
{
    private :

        ServerContext                           & server_context ;
        Request                                 & request ;
        std::string                             message ;
        bool                                    _end_of_response ;
        bool                                    _tranfer_encoding ;
        bool                                    is_first_message ;
        std::ifstream                           page_content ;
        LocationContext *                       _location;
        short                                   status;
        std::map< std::string , std::ofstream > files_to_upload ;
        bool                                    _running_post ;

        void                                    format_message( void ) ;
        bool                                    is_cgi( const std::string & path , const std::string & cgi_extention );
        bool                                    is_allowd_method() ;
        bool                                    is_allowd_method_in_location();
        void                                    get_static_page() ;
        void                                    generate_message( char * content, size_t size ) ;
        LocationContext *                       find_location( const std::string & target ) ;
        LocationContext *                       find_exact_location( const std::string &target ) ;
        void                                    remove_last_slash( std::string & target ) ;
        bool                                    get_path_of_page( std::string & path_of_page, std::string & cgi_extention ) ;
        bool                                    path_from_location(std::string & path_of_page, std::string & cgi_extention , std::string & target, std::string & new_target);
        bool                                    path_from_root(std::string & path_of_page, std::string & cgi_extention , std::string & target );
        
        // redirection          
        void                                    redirection_handler( unsigned short status, const std::string & msg_or_file ) ;
        LocationContext *                       find_match_more_location( std::string & new_target ) ;
            
        // error            
        std::string                             find_error_page( unsigned  short error ) ;
        void                                    error_response( short error ) ;
        void                                    responde_with_default_page( short error ) ;
        void                                    responde_with_overrided_page( short error, std::string err_page_path ) ;
            
        // auto index           
        void                                    respond_list_files( const std::string & path , const std::string & target) ;
        bool                                    is_folder( const std::string & path ) ;
        void                                    append_row( std::string  path , std::string target, struct dirent * f, std::string & ls_files ) ;
                    
        // cgi          
        void                                    execute_cgi() ;
            
        // methods          
        void                                    delete_file( const std::string & path ) ;
            
        // upload files         
        void                                    upload_data(const std::string & file_name, const std::string & data );
        void                                    post_data() ;
        
        
        
    public:

        Response ( ServerContext & server_context, Request & request ) ;

        /* Getters */
        const std::string &         getResponse  ( void ) ;
        const std::ifstream &       getPageStream( void ) ;

        bool                        end_of_response() ;
        bool                        tranfer_encoding() ;

        ~Response() ;

};

#endif
// /about/index.html