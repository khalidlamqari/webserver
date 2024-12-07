/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:57 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/07 15:00:47 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../includes/main.hpp"
# include "DefaultInfo.hpp"
# include <sys/stat.h>
# include <dirent.h>
#include <time.h>
#include <fstream>
#include "../Request/Request.hpp"
class Request ;
static DefaultInfo default_info ;

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
        
        std::string                             _cgi_extention ;
        bool                                    _is_cgi ;
        int                                     s_fds[2] ;
        int                                     fd_input;
        std::string                             path_input;
        int                                     exit_stat ;
        pid_t                                   pid ;
        std::string                             connection;
        std::string                             unparsed_content;
        std::string                             data_out;
        size_t                                  offset;
        
        

        
        void                                    format_message( void ) ;

        void                                    get_static_page() ;
        void                                    generate_message( char * content, size_t size ) ;
        LocationContext *                       find_location( const std::string & target ) ;
        LocationContext *                       find_exact_location( const std::string &target ) ;
        void                                    remove_last_slash( std::string & target ) ;
        bool                                    get_path_of_page() ;
        bool                                    path_from_location();
        bool                                    path_from_root();
        void                                    read_and_format_msg();
        void                                    get_pathinfo_form_target();
        void                                    normall_headers(char * content, size_t size );
        void                                    tranfer_encod_headers();
        // redirection          
        void                                    redirection_handler( unsigned short status, const std::string msg_or_file ) ;
        LocationContext *                       find_match_more_location( std::string target ) ;
            
        // error            
        std::string                             find_error_page( unsigned  short error ) ;
        void                                    error_response( short error ) ;
        void                                    responde_with_default_page( short error ) ;
        void                                    responde_with_overrided_page( short error, std::string err_page_path ) ;

        // auto index           
        void                                    respond_list_files() ;
        void                                    append_row( std::string  path , std::string target, struct dirent * f, std::string & ls_files ) ;

        // cgi
        void                                    setup_environment(std::vector<std::string> & string_env);
        void                                    format_cgi_msg();

        // methods
        void                                    delete_file( ) ;

        void                                    process_target(const std::string & target);
        void                                    parse_headers();
        void                                    get_response_body();
        
    public:

        Response ( ServerContext & server_context, Request & request ) ;

        /* Getters */
        const std::string &         get_connection() const;
        const std::string &         getResponse  (void);
        const std::string &         getUploadDir  (void);
        const std::ifstream &       getPageStream(void);
        bool                        end_of_response();
        bool                        tranfer_encoding();
        int *                       get_pair_fds();
        pid_t                       get_process_id();
        int                         get_exit_stat();
        LocationContext *           get_location();

        // cgi
        void                                    read_cgi_output() ;
        void                        execute_cgi();
        bool                        is_cgi();
        bool                        p_is_running;
        bool                        is_parsed;
        void                        set_exit_stat(int stat);
        void                        set_end_of_response(bool stat);
        int                         get_parse_stat();
        void                        set_parse_stat(int stat);
        bool                        is_allowd_method();
        std::string                 data_path;

        ~Response() ;

};

#endif
