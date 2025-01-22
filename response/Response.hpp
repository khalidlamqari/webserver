
#pragma once
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "includes/main.hpp"
class CgiPairSocket;
class CgiProcess;
class Request ;
class ClientSocket;

class Response
{
    private :
        std::string                             message ;
        bool                                    _end_of_response ;
        bool                                    _tranfer_encoding ;
        bool                                    is_first_message ;
        std::ifstream                           page_content ;
        unsigned short                          status;
        std::string                             _upload_dir ;
        bool                                    _running_post ;
        std::string                             _path_ ;
        std::string                             _path_info ;
        std::string                             _target ;

        std::string                             _cgi_extention ;
        std::string                             exec_path ;
        bool                                    _is_cgi ;
        int                                     fd_input;
        std::string                             path_input;
        int                                     exit_stat ;

        std::string                             unparsed_content;
        std::string                             data_out;
        size_t                                  offset;

        std::string                             input_path;
    
        CgiProcess*		_cgi_process;
		CgiPairSocket* 	_cgi_pair_socket;

        void                                    format_response() ;

        LocationContext *                       find_location( const std::string & target ) ;

        void                                    format_static_response();

        // redirection          
        void                                    redirection_handler() ;

        void                                    find_match_more_location();

        // error            
        std::string                             find_error_page( unsigned  short error ) ;
        void                                    error_response( short error ) ;
        void                                    responde_with_default_page( short error ) ;
        void                                    responde_with_overrided_page( std::string err_page_path ) ;

        // auto index           
        void                                    directory_listing() ;

        // cgi
        void                                    format_cgi_response();

        // methods
        void                                    delete_file( ) ;

        void                                    parse_headers();

        



    public:

        bool            is_processed;
        const ServerContext                     * server_context ;
        LocationContext *                       _location;
        size_t                                  body_size;
        Response ( ClientSocket & client_info );
        /* Getters */
        void format_start_line();
        void  format_headers(size_t size);
        void   format_body(char * content, size_t size);
        ClientSocket                            & client_info ;

        const std::string &         getResponse  (void);
        const std::string &         getUploadDir  (void);
        const std::ifstream &       getPageStream(void);
        bool                        end_of_response();
        bool                        tranfer_encoding();

        int                         get_exit_stat();
        LocationContext *           get_location();

        void                        process_request();

        // cgi
        void                        read_cgi_output() ;

        bool                        is_cgi();
        //

        bool                        p_is_running;

        void                        set_exit_stat(int stat);
        void                        set_end_of_response(bool stat);
        int                         get_status();
        void                        set_status(int stat);
        bool                        is_allowed_method();
        // void                        set_start_time(std::time_t tm);
        // std::time_t                 get_start_time();
        const std::string &         get_input_path();
        void                        set_input_path(const std::string & path);
        void                        set_exec_path(const std::string & exec);

        // ADD NEW
        void                        set_path(const std::string & path);
        void                        set_cgi_extention(const std::string & ext);
        void                        set_upload_dir(const std::string & dir);
        void                        set_target(const std::string & target);

        void                        extract_pathinfo_form_target();

        const std::string &         get_target(void) const;
        const std::string &         get_path(void) const;
        const std::string &         get_cgi_exrention(void) const;
        const std::string &         get_upload_dir() const;
        void                        get_exec_path( char * av[]) ;
        const std::string &         get_path_info(void) const;
        std::string &               get_data_out(void);
        
        
        CgiProcess*		get_cgi_process(void);
		CgiPairSocket* 	get_cgi_pair_socket(void);
        void            set_cgi_process(CgiProcess* proc);
        void            set_cgi_pair_socket(CgiPairSocket* cgi_sock);
        void            set_is_cgi(bool is_cgi);
        
        ~Response() ;

};

bool    checkBrokenIdents(int ident, std::string cmd);

#endif
