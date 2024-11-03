/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:57 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/03 12:47:15 by klamqari         ###   ########.fr       */
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
        bool                        _end_of_response ;
        bool                        _tranfer_encoding ;
        bool                        is_first_message ;
        std::ifstream               page_content ;
        LocationContext *           _location;

        void                        format_message( void ) ;
        bool                        is_cgi_request() ;
        bool                        is_allowd_method() ;
        void                        get_static_page() ;
        void                        generate_message( char * content, size_t size ) ;
        LocationContext *           find_location( const std::string & target ) ;
        LocationContext *           find_exact_location( const std::string &target ) ;
        void                        remove_last_slash( std::string & target ) ;
        // redirection 
        void                        redirection_handler( unsigned short status, const std::string & msg_or_file ) ;
        LocationContext *           find_match_more_location( std::string & new_target );
        
        // error
        std::string                 find_error_page( unsigned  short error ) ;
        void                        error_response( short error ) ;
        
    public:

        Response ( ServerContext & server_context, Request & request ) ;

        /* Getters */

        const std::string &         getResponse( void );
        bool                        end_of_response() ;
        bool                        tranfer_encoding() ;

        ~Response() ;

};

#endif