/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 11:35:53 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/11 10:40:49 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef REQUEST_HPP

#define REQUEST_HPP

# include "../includes/main.hpp"

/*
    HTTP-message = start-line
                    *( header-field CRLF )
                    CRLF
                    [ message-body ]

*/

class Request
{
    private :
        std::string message ;
        // status-line = <HTTP Method>  <Request-URI>  <HTTP Version>
        // request-line = method SP request-target SP HTTP-version CRLF
        void parseStartLine(const std::string & line) ;
        bool Valide_method( const std::string methodName );
        
        void parseHeader(const std::string & line ) ;
        

        
    public :
        std::map< std::string, std::string > headers ;// key , value
        std::string method ;
        std::string request_target ;
        std::string HTTP_version ;
        // Request() = delete ;
        Request(std::string message) ;
        
        void parseMessage() ;
        
        ~Request() ;

};

# endif