/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Redirections.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 22:33:17 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/30 12:18:26 by klamqari         ###   ########.fr       */
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
    std::string host = "http://127.0.0.1:8888";

    if ( (status > 300 && status < 304) || 307 == status || 308 == status )
    {
        size_t pos = msg_or_file.find("http://");
        if (pos != 0 && msg_or_file.front() != '/')
        {
            msg_or_file = host + "/" + msg_or_file;
        }
        else if (pos != 0)
        {
            msg_or_file = host + msg_or_file;
        }
        std::cout << "msg_or_file " << msg_or_file << std::endl;
        this->message.append( "HTTP/1.1 "  + default_info.getCodeMsg( status ) + "\r\n" ) ;
        this->message.append( "Location: " + msg_or_file + "\r\nContent-Length: 0\r\nConnection: Keep-alive\r\n\r\n" ) ;
    }
    else
    {
        ss << msg_or_file.length() ;
        this->message.append( "HTTP/1.1 "  + default_info.getCodeMsg( status ) + "\r\n" ) ;
        this->message.append("Content-Length: " + ss.str() + "\r\nConnection: Keep-alive\r\n\r\n" + msg_or_file );
    }
    this->_end_of_response = true ;
}

/*
https://datatracker.ietf.org/doc/html/rfc7231
*/