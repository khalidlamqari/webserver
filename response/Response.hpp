/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:57 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/24 15:38:30 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "../includes/main.hpp"
# include "../request/Request.hpp"

class Response
{
    private :

        ServerContext               & server_context ;

        Request                     & request ;

        int                         error_number ;
        
        std::string                 message ;

        void                        format_message( void ) ;
    
    public:

        Response ( ServerContext & server_context, Request & request, int error_number) ;
        
        /* Getters */

        const std::string getResponse( void ) const;

        ~Response() ;

};

#endif