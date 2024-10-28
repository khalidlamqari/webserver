/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:57 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/28 13:29:59 by klamqari         ###   ########.fr       */
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
        std::string                         message ;
        void                        format_message( void ) ;
        void                        error_response( short error ) ;
        bool                        is_cgi_request() ;

    public:

        Response ( ServerContext & server_context, Request & request ) ;

        /* Getters */

        const std::string & getResponse( void );

        ~Response() ;

};

#endif