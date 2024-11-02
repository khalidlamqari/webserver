/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:57 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/02 09:19:45 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../includes/main.hpp"

class Request ;

class Response
{
    private :

        ServerContext               & server_context ;
        Request                     & request ;
        std::string                 message ;
        bool                        is_finished ;

        void                        format_message( void ) ;
        void                        error_response( short error ) ;
        bool                        is_cgi_request() ;
        bool                        is_allowd_method() ;
        void                        get_static_page() ;
        void                        generate_message( char * content, size_t size ) ;
        LocationContext *           find_location( const std::string & target ) ;
        void                        remove_last_slash( std::string & target ) ;
        // redirection 
        void                        redirection_handler( unsigned short status, const std::string & msg_or_file ) ;
    public:

        Response ( ServerContext & server_context, Request & request ) ;

        /* Getters */

        const std::string & getResponse( void );

        ~Response() ;

};

#endif