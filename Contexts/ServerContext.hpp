
#ifndef SERVER_CONTEXT_HPP
# define SERVER_CONTEXT_HPP

#include <vector>
#include <string>
#include <iostream>
#include <map>

#include "LocationContext.hpp"

typedef std::string extension;
typedef std::string execPath;

class ServerContext {

	public :
		
		/*  Flags to detect duplications in configuration file. */
		
		bool    srv_names_is_set;
		bool    auto_ind_is_set;
		bool    methods_is_set;
		bool    upl_dir_is_set;
		bool    index_is_set;
		bool    port_is_set;
		bool    root_is_set;
		bool    host_is_set;
		bool	cgi_read_timeout_is_set;
		bool    cgi_info_inherited;


		/* ServerContext Constructor */
		ServerContext( const std::map<extension, execPath>& CgiExecPaths );

		/* ServerContext Destructor */
		~ServerContext();

		/* Setters */
		void    set_error_page      ( const t_error_page& error_info );
		void    set_new_location    ( const std::string & location );
		void    set_ports           ( std::vector<unsigned short> ports );
		void    set_root_directory  ( std::string root );
		void    set_upload_dir      ( std::string directory );
		void    set_index           ( std::string index );
		void    set_server_names    ( std::vector<std::string> names );
		void    set_auto_index      ( const std::string & on_off );
		void    set_allowed_methods ( std::vector<std::string> methods );
		void    set_host            ( std::string host );
		void    set_max_body_size   ( const size_t & size );
		void    set_cgi_info		( const std::pair<extension, execPath>& info );
		void	set_cgi_rd_tm_out	( time_t max_time );

		/* The following method returns the latest inserted locationContext in the locations vector in order to store it's info. */
		LocationContext&  get_latest_location( void );

		/* Getters */

		const std::vector<t_error_page>&    	get_error_pages( void ) const;
		const std::vector<LocationContext>&		get_locations( void ) const;
		const std::vector<std::string>&    		get_server_names( void ) const;
		const std::vector<std::string>&    		get_allowed_methods( void ) const;
		const std::vector<unsigned short>& 		get_ports( void ) const;
		const std::map<extension, execPath>&    get_cgi_info( void ) const;
		const std::string&                 		get_root_directory( void ) const;
		const std::string&                 		get_upload_dir( void ) const;
		const std::string&                 		get_index( void ) const;
		const std::string&                 		get_host( void ) const;
		const bool&                        		get_auto_index( void ) const;
		const size_t&                           get_max_body_size( void ) const;
		const time_t&							get_cgi_read_time_out( void ) const;

		/* Methods */
		void                                    clear_cgi_info( void );
		void									show_server_names( void ) const;

	private :

		std::vector<t_error_page>       error_pages;

		std::vector<LocationContext>    locations;

		std::vector<std::string>        server_names;

		std::vector<std::string>        allowed_methods;

		std::vector<unsigned short>     ports;

		std::map<extension, execPath>	CgiExecPaths;

		std::string                     root_directory;

		std::string                     upload_dir;

		std::string                     index;

		std::string                     host;

		bool                            auto_index;
		
		size_t						    max_body_size;

		time_t							cgi_read_timeout;
};

#endif
