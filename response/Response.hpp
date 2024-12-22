/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:57 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/22 13:03:31 by klamqari         ###   ########.fr       */
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
#include <ctime>
#include <fstream>
#include "../Request/Request.hpp"
#include "../Utils/utils.hpp"
#include "../server_setup/Socket.hpp"

class CgiPairSocket;
class CgiProcess;
class Request ;
class ClientSocket;

static DefaultInfo default_info ;

class Response
{
    private :
        const ServerContext                     * server_context ;
        // Request                                 & request ;
        ClientSocket                            & clientsocket ;
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
        
        std::string                             unparsed_content;
        std::string                             data_out;
        size_t                                  offset;

        std::time_t                             start_time;
        std::string                             input_path;
        std::ofstream                           input_data;
        
        CgiProcess*		_cgi_process;
		CgiPairSocket* 	_cgi_pair_socket;
        
        void                                    format_message( void ) ;
        void                                    get_static_page() ;
        void                                    generate_message( char * content, size_t size ) ;
        LocationContext *                       find_location( const std::string & target ) ;
        // LocationContext *                       find_exact_location( const std::string &target ) ;
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
        // LocationContext *                       find_match_more_location( std::string target ) ;
        void                find_match_more_location();
            
        // error            
        std::string                             find_error_page( unsigned  short error ) ;
        void                                    error_response( short error ) ;
        void                                    responde_with_default_page( short error ) ;
        void                                    responde_with_overrided_page( std::string err_page_path ) ;

        // auto index           
        void                                    respond_list_files() ;
        void                                    append_row( std::string  path , std::string target, struct dirent * f, std::string & ls_files ) ;

        // cgi
        void                                    setup_environment(std::vector<std::string> & string_env);
        void                                    format_cgi_msg();

        // methods
        void                                    delete_file( ) ;

        void                                    process_requset();
        void                                    parse_headers();
        void                                    get_response_body();
        
    public:

        // Response ( const ServerContext & server_context, Request & request ) ;
        Response ( ClientSocket & clientsocket );
        /* Getters */

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
        int                         get_status();
        void                        set_status(int stat);
        bool                        is_allowed_method();
        void                        set_start_time(std::time_t tm);
        std::time_t                 get_start_time();
        const std::string &         get_input_path();
        void                        set_input_path(const std::string & path);
        void                        set_data_to_input(const std::string & data);

        // ADD NEW
        void                        set_path(const std::string & path);
        void                        set_cgi_extention(const std::string & ext);
        void                        set_upload_dir(const std::string & dir);
        void                        set_target(const std::string & target);
        
        const std::string &         get_target(void) const;
        const std::string &         get_path(void) const;
        
        
        
        CgiProcess*		get_cgi_process(void);
		CgiPairSocket* 	get_cgi_pair_socket(void);
        void            set_cgi_process(CgiProcess* proc);
        void            set_cgi_pair_socket(CgiPairSocket* cgi_sock);
        
        ~Response() ;

};

#endif
