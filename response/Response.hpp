/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:57 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/29 16:46:16 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "../includes/main.hpp"
# include "../request/Request.hpp"
# include "ErrorPages.hpp"

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
        std::vector<LocationContext>::iterator find_location( const std::string & target );

    public:

        Response ( ServerContext & server_context, Request & request ) ;

        /* Getters */

        const std::string & getResponse( void );

        ~Response() ;

};

#endif