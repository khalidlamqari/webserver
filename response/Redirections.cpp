/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Redirections.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 22:33:17 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/21 15:44:12 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

/*
**   https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
**   https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
**    HTTP/1.1 301 Moved Permanently
**    Location: http://example.com/new-location
**    Content-Length: 0
**    Connection: keep-alive

**     301 (Moved Permanently)
**     302 (Found / Moved Temporarily)
**     303 (See Other)
**     307 (Temporary Redirect)
**     308 (Permanent Redirect)
*/

void    Response::redirection_handler( unsigned short status, std::string msg_or_file )
{
    std::stringstream ss;

    if ( (status > 300 && status < 304) || 307 == status || 308 == status )
    {
        this->message.append( "HTTP/1.1 "  + default_info.getCodeMsg( status ) + "\r\n" ) ;
        this->message.append( "Location: " + msg_or_file + "\r\nContent-Length: 0\r\nConnection: "\
        + this->connection + "\r\n\r\n" ) ;
    }
    else
    {
        ss << msg_or_file.length() ;
        this->message.append( "HTTP/1.1 "  + default_info.getCodeMsg( status ) + "\r\n" ) ;
        this->message.append("Content-Length: " + ss.str() + "\r\nConnection: "\
        + this->connection + "\r\n\r\n" + msg_or_file );
    }
    this->_end_of_response = true ;
}

/*
https://datatracker.ietf.org/doc/html/rfc7231
*/