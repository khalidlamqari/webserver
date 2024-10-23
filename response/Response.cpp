/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 23:40:37 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/24 00:14:31 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"


Response::Response ( ServerContext & server_context, Request & request) :\
                    server_context(server_context), request(request)
{
}

/* Getters */

const std::string Response::getResponse( void ) const
{
    (void)this->request ;
    (void)this->server_context ;
    return "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<h1 style='color:red;'>Hello from C++ server!</h1>" ;
}

Response::~Response() 
{
    
}
