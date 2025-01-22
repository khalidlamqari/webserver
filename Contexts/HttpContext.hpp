
#ifndef HTTP_CONTEXT_HTTP
# define HTTP_CONTEXT_HTTP

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "ServerContext.hpp"

class HttpContext {

	public :

		/* HttpContext Constructor */
		HttpContext ( void );

		/* HttpContext Destructor */
		~HttpContext ();

		/* Setters */
		void    set_error_page		( const t_error_page& error_page );
		void    set_new_server		( void ); 							//  This method creates a new Server Context when a new server directive is found in the configuration file.
		void    set_cgi_info		( const std::pair<extension, execPath>& info );
		void	set_auto_index		( const std::string & on_off);
		void    set_max_body_size	( size_t limit );
		void	set_cgi_rd_tm_out	( time_t max_time );

		/* Getters */
			/* The following method returns the latest inserted serverContext in the servers vector in order to store it's info. */
		ServerContext&						    get_latest_server( void );

		const std::vector<ServerContext>&	    get_servers( void ) const;
		const std::vector<t_error_page>&	    get_error_pages( void ) const;
		const std::map<extension, execPath>&    get_cgi_info( void ) const;
		const bool&							    get_auto_index( void ) const;
		const time_t&							get_cgi_read_time_out( void ) const;

		/* Flags to avoid duplicated directives */
		bool	auto_ind_is_set;
		bool	max_body_is_set;
		bool	cgi_read_timeout_is_set;

	private :

		std::vector<t_error_page>	    error_pages;

		std::vector<ServerContext>	    servers;

		std::map<extension, execPath>	CgiExecPaths;

		size_t						    max_body_size;

		bool						    auto_index;

		time_t							cgi_read_timeout;
};


#endif