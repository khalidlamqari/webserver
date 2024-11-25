/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:57 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/25 09:52:37 by klamqari         ###   ########.fr       */
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
#include "../Request/Request.hpp"
class Request ;
class ErrorPage;
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
        std::string                             _upload_dir ;
        bool                                    _running_post ;
        std::string                             _path_ ;
        std::string                             _path_info ;
        std::string                             _target ;
        std::string                             _new_target;
        
        std::string                             _cgi_extention ;
        bool                                    _is_cgi ;
        int                                     s_fds[2] ;
        int                                     exit_stat ;
        pid_t                                   pid ;
        

        
        void                                    format_message( void ) ;
        bool                                    is_allowd_method() ;
        bool                                    is_allowd_method_in_location();
        void                                    get_static_page() ;
        void                                    generate_message( char * content, size_t size ) ;
        LocationContext *                       find_location( const std::string & target ) ;
        LocationContext *                       find_exact_location( const std::string &target ) ;
        void                                    remove_last_slash( std::string & target ) ;
        bool                                    get_path_of_page() ;
        bool                                    path_from_location( std::string & target, std::string & new_target);
        bool                                    path_from_root( std::string & target );
        void                                    read_and_format_msg();
        void                                    get_pathinfo_form_target();
        // redirection          
        void                                    redirection_handler( unsigned short status, const std::string & msg_or_file ) ;
        LocationContext *                       find_match_more_location( std::string & new_target ) ;
            
        // error            
        std::string                             find_error_page( unsigned  short error ) ;
        void                                    error_response( short error ) ;
        void                                    responde_with_default_page( short error ) ;
        void                                    responde_with_overrided_page( short error, std::string err_page_path ) ;

        // auto index           
        void                                    respond_list_files( const std::string & target) ;
        bool                                    is_folder( const std::string & path ) ;
        void                                    append_row( std::string  path , std::string target, struct dirent * f, std::string & ls_files ) ;

        // cgi
        // void                                    execute_cgi() ;
        void                                    read_cgi_output() ;
        void                                    setup_environment(char ***env);

        // methods          
        void                                    delete_file( ) ;
        void                                    head();

        // upload files         
        // void                                    upload_data(const std::string & file_name, const std::string & data );
        void                                    post_data() ;

        bool                                    process_target();
        
        
    public:

        Response ( ServerContext & server_context, Request & request ) ;

        /* Getters */
        const std::string &         getResponse  ( void ) ;
        const std::string &         getUploadDir  ( void ) ;
        const std::ifstream &       getPageStream( void ) ;

        bool                        end_of_response() ;
        bool                        tranfer_encoding() ;
        // cgi
        void                        execute_cgi() ;
        bool                        is_cgi( );
        
        int *                       get_pair_fds();
        pid_t                       get_process_id();
        bool                        p_is_running;
        int                         get_exit_stat();
        void                        set_exit_stat(int stat);
        ~Response() ;

};

#endif
// /about/index.html