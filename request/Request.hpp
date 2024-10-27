/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 11:35:53 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/27 12:28:34 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef REQUEST_HPP

#define REQUEST_HPP

# include "../includes/main.hpp"

/*
*    HTTP-message = start-line
*                    *( header-field CRLF )
*                    CRLF
*                    [ message-body ]
*
*/

class Request
{
    private :
        std::string message ;
        // status-line = <HTTP Method>  <Request-URI>  <HTTP Version>
        // request-line = method SP request-target SP HTTP-version CRLF
        void                        parseStartLine(const std::string & line) ;
        bool                        Valide_method( const std::string methodName );
        
        void                        parseHeader(const std::string & line ) ;
        std::vector<std::string>    parse_header_values(const std::string & field_value);
        void check_valid_headres();
        
    public :
        std::map< std::string, std::vector<std::string> >   headers ;// key , value
        
        std::string                                         method ;
        std::string                                         request_target ;
        std::string                                         HTTP_version ;
        std::string                                         body ;
        size_t                                              content_length;
        size_t                                              counter_recv;
        bool                                                isReady ;
        
        Request(std::string message) ;
        Request() ;
        
        void                                                parseMessage() ;
        
        /* Setters */
        
        void setMessage( std::string message ) ;
        const std::string   &  getMessage( void ) ;
        void appendMessage(  char * message , ssize_t             bytes_received ) ;
        void appendTobody(  char * message , ssize_t             bytes_received ) ;
        

        /* Getters */

        const std::string                                           & get_request_method( void ) const ;
        const std::string                                           & get_request_target( void ) const ;
        const std::string                                           & get_request_body  ( void ) const ;
        std::map < std::string, std::vector<std::string> >          & get_request_headers( void ) ;
        void                                                        reuse();

        
        ~Request() ;

};

# endif