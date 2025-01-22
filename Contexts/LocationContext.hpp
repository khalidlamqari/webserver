
#ifndef LOCATION_CONTEXT_HPP
# define LOCATION_CONTEXT_HPP

#include <vector>
#include <string>
#include <iostream>
#include <map>

#include "../config_file_parsing/config_structs.hpp"

typedef std::string extension;
typedef std::string execPath;

class LocationContext {

	public :

	/*  Flags to detect duplications in configuration file. */

	bool	redirect_is_set;
	bool	auto_ind_is_set;
	bool	methods_is_set;
	bool	upl_dir_is_set;
	bool	index_is_set;
	bool	root_is_set;
	bool	alias_is_set;
	bool	cgi_info_inherited;

	/* LocationContext Constructor */
	LocationContext( void );
	LocationContext( const std::string & location, const std::map<extension, execPath>& CgiExecPaths );

	/* LocationContext Destructor */
	~LocationContext();

	/* Setters */

	void    set_error_page      ( const t_error_page & error_info );
	void    set_redirection     ( t_redirection_info redirection_info );
	void    set_root_directory  ( std::string root );
	void    set_upload_dir      ( std::string directory );
	void    set_index           ( std::string index );
	void    set_auto_index      ( const std::string & on_off );
	void    set_allowed_methods ( std::vector<std::string> methods );
	void    set_location        ( const std::string & location );
	void    set_alias	        ( const std::string & alias );
	void    set_cgi_info		( const std::pair<extension, execPath>& info );

		/* Getters */

	const std::vector<t_error_page> &							get_error_pages    	( void ) const;
	const t_redirection_info &									get_redirection    	( void ) const;
	const std::vector<std::string>&								get_allowed_methods	( void ) const ;
	const std::string&											get_root_directory 	( void ) const ;
	const std::string&											get_upload_dir     	( void ) const ;
	const std::string&											get_index          	( void ) const ;
	const std::string&											get_location       	( void ) const ;
	const std::string&											get_alias			( void ) const ;
	const bool &												get_auto_index     	( void ) const ;
	const std::map<extension, execPath>&						get_cgi_info		( void ) const;

	/* Methods */
	void                                    					clear_cgi_info( void );

	private :

		std::vector<t_error_page>                               error_pages;

		t_redirection_info                                      redirection;

		std::vector<std::string>                                allowed_methods;

        std::map<extension, execPath>							CgiExecPaths;

		std::string                                             location;

		std::string                                             root_directory;

		std::string                                             upload_dir;

		std::string                                             index;

		std::string												alias;	

		bool                                                    auto_index;

};


#endif