/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 11:24:24 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/11 19:20:23 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Request.hpp"

Request::Request(std::string message)
{
    this->message = message ;
    this->content_length = 0;
    this->counter_recv = 0;
    this->isReady = false ;
    this->status = 200 ;
}

/*
*
*    CR (carriage return), CRLF (CR LF) LF (line feed)
*
*    typically parse the request-line into its component parts
*    by splitting on whitespace . since no whitespace is allowed in the three components
*
*    -- ERROR--
*    Recipients of an invalid request-line SHOULD respond with either a
*    400 (Bad Request) error or a 301 (Moved Permanently)
*
*/


void Request::parseStartLine(const std::string & line) 
{
    std::cout << "start line : " << line << std::endl;
    std::vector<std::string> tokens ;
    if ( line[ line.length() - 1 ] != '\r' )
        throw 400 ;
    tokens = split(line) ;
    std::vector<std::string>::iterator start = tokens.begin();
    // std::vector<std::string>::iterator end = tokens.end();

    // start line should be 3 elements <HTTP Method>  <Request-URI>  <HTTP Version>
    if ( tokens.size() != 3 )
        throw 400 ;

    //  <HTTP Method>
    // if ( ! this->Valide_method(*start) )
    //     throw 400 ;
    this->method = *start ;

    // <Request-URI>
    this->request_target = *(++start);

    // <HTTP Version>
    if ( (*(++start)) != "HTTP/1.1" ) // should trim(*start first)
        throw 400 ;
    this->HTTP_version = "HTTP/1.1" ;

    // while ( start != end )
    // {
    //     std::cout << "|" << *start << "|" << std::endl;
    //     start++;
    // }

}


/*
*   body
*   a Content-Length header  field is normally sent in a POST request even when the value is 0  (indicating an empty payload body).
*   
*   A user agent SHOULD NOT send a  Content-Length header field 
*   when the request message does not contain  a payload body 
*   and the method semantics do not anticipate such a  body.
*/

void Request::parseMessage()
{
    std::string line ;
    std::istringstream stream(this->message) ; // Treat the string as an input stream 
    size_t size = 0 ; 
    short   were_am_i = 0 ;
                        /*
                        * 0 = start line 
                        * 1 = header line
                        * 2 = body line
                        */
 
    while ( getline( stream , line ) )
    {
        if ( 0 == were_am_i )
        {
            parseStartLine( line ) ;
            were_am_i = 1 ;
        }
        else if ( 1 == were_am_i )
        {
            if (line.length() == 1 && line[0] == '\r') // CRLF
                were_am_i = 2 ;
            else
                parseHeader( line ) ;
        }
        else if ( 2 == were_am_i ) // read the body
        {
            this->body.resize( this->message.length() - size - 1) ;
            stream.read( &this->body[0], (this->message.length() - size - 1) ) ;
        }
        size = size + line.length() + 1;
    }
    // if ( 2 != were_am_i )
    //     throw 400 ;
    // check_valid_headres() ;

}

bool Request::Valide_method( const std::string methodName )
{
    std::string methods[4] = { "GET" , "POST" , "HEAD" , "DELETE" } ;

    for ( size_t i = 0 ; i < methods->length() ; i++ )
        if ( methods[i] == methodName )
            return ( true ) ;
            
    return ( false ) ;
}

/*
*   A user agent SHOULD NOT send a  Content-Length header field when the request message does not contain  a payload body
*   
*   A server MAY reject a request that contains a message body but not a  Content-Length by responding with 
*   411 (Length Required).
*/

void Request::check_valid_headres()
{
    std::map< std::string , std::vector<std::string> >::iterator header ;
 
    // header = this->get_request_headers();
    
    header = this->headers.find("Host") ;
    if ( header == this->headers.end() ) // should check the value
        throw 400 ;
    else
    {
        if ( header->second.size() != 1 )
            throw 400;
    }

    header = this->headers.find("Content-Length") ;
    
    if ( header != this->headers.end() )
    {
        if ( header->second.size() != 1  ||  ! is_all_digit((*(header->second.begin()))) || (std::atoi( (*(header->second.begin())).c_str() )) <  0 )
        {
            std::cout << "error in content lenght header " << std::endl;
            throw 411 ;
        }
        this->content_length = std::atoi( (*(header->second.begin())).c_str() ) ;
        if (  this->body.length() != (size_t) std::atoi( (*(header->second.begin())).c_str() ) )
        {
            std::cout << "error in content lenght header " << std::endl;
            throw 411 ;
        }
    }
    else
    {
        if ( ! this->body.empty() )
            throw 411 ; // 411 (Length Required).
    }
}

Request::Request() {}

/* Setters */

void Request::setMessage( std::string message )
{
    std::cout << "message : \n" << message << std::endl;
    this->message = message ;
}

void Request::appendMessage ( char * message , ssize_t bytes_received)
{
    // (void)bytes_received ;
    if ( bytes_received > 0 )
        this->message.append( message, bytes_received ) ;
}

void Request::appendTobody(  char * message , ssize_t bytes_received )
{
    this->body =  this->message.substr( bytes_received + 4 ) ;
    std::cout << this->body << std::endl;
    // if ( bytes_received > 0 )
    //     this->body.append( message, bytes_received );
    (void)message ;
}

void Request::setStatus( short status )
{
    this->status = status ;
}

/* getters */

const std::string   &  Request::getMessage( void )
{
    return ( this->message ) ;
}

const std::string   & Request::get_request_method( void ) const
{
    return ( this->method ) ;
}

const std::string   & Request::get_request_target( void ) const
{
    return ( this->request_target ) ;
}

const std::string   & Request::get_request_body  ( void ) const
{
    return ( this->body ) ;
}

std::map < std::string, std::vector<std::string> >    & Request::get_request_headers( void )
{
    return ( this->headers ) ;
}

short  Request::getStatus( void )
{
    return ( this->status ) ;   
}


void   Request::reuse()
{
    this->message = "";
    this->body = "";
    this->method = "" ;
    this->HTTP_version = "" ;
    this->request_target = "" ;
    this->isReady = false ;
    std::map< std::string, std::vector<std::string> >   headers ;
    this->headers = headers ;
    this->content_length = 0;
    this->status = 200 ;
}


Request::~Request() 
{
}