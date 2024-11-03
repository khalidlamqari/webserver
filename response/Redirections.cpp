/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Redirections.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 22:33:17 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/03 11:47:32 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/main.hpp"

/*
**    HTTP/1.1 301 Moved Permanently
**    Location: http://example.com/new-location
**    Content-Length: 0
**    Connection: keep-alive
*/

void    Response::redirection_handler( unsigned short status, const std::string & msg_or_file )
{
    std::stringstream ss;

    if ( (status > 300 && status < 304) || 307 == status || 308 == status )
    {
        this->message.append( "HTTP/1.1 "  + default_error_pages.getErrorMsg( status ) + "\r\n" ) ;
        this->message.append( "Location: " + msg_or_file + "\r\nContent-Length: 0\r\nConnection: Keep-alive\r\n\r\n" ) ;
    }
    else
    {
        if ( msg_or_file.empty() )
            this->message.append( "HTTP/1.1 "  + default_error_pages.getErrorMsg( status ) + "\r\nContent-Length: 0\r\nConnection: Keep-alive\r\n\r\n" ) ;
        else
        {
            ss << msg_or_file.length() ;
            this->message.append( "HTTP/1.1 "  + default_error_pages.getErrorMsg( status ) + "\r\n" ) ;
            this->message.append("Content-Length: " + ss.str() + "\r\nConnection: Keep-alive\r\n\r\n" + msg_or_file );
        }
    }
    this->_end_of_response = true ;
}
