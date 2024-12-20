/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpContext.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:31:07 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/30 00:43:20 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CONTEXT_HTTP
# define HTTP_CONTEXT_HTTP

#include <vector>
#include <string>
#include <iostream>

#include "ServerContext.hpp"

class HttpContext {

    public :

        /* HttpContext Constructor */
        HttpContext ( void );

        /* HttpContext Destructor */
        ~HttpContext ();

        /* Setters */
        void    set_error_page      ( const t_error_page& error_page );
        void    set_new_server      ( void ); //  This method creates a new Server Context when a new server directive is found in the configuration file.
        void    set_cgi_extension   ( const std::string& extension );
		void	set_auto_index		( const std::string & on_off);
        void    set_max_body_size   ( size_t limit ); // TODO : The max body size has to be used 

        /* Getters */


        /* The following method returns the latest inserted serverContext in the servers vector in order to store it's info. */
        ServerContext&						get_latest_server( void );

        /* Getters */
        const std::vector<ServerContext>&	get_servers( void ) const;
        const std::vector<t_error_page>&	get_error_pages( void ) const;
        const std::string&					get_cgi_extension( void ) const;
        const bool&							get_auto_index( void ) const;


        /* Flags to avoid duplicated directives */
		bool	cgi_ext_is_set;
		bool	auto_ind_is_set;
		bool	max_body_is_set;

    // private :

		std::vector<t_error_page>	error_pages; // TODO : instead of using pair make a struct

		std::vector<ServerContext>	servers;

		std::string					cgi_extension;

		size_t						max_body_size;

		bool						auto_index;

};


#endif