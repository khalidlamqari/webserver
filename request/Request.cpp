/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 11:24:24 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/14 16:27:45 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Request.hpp"

Request::Request(std::string message)
{
    this->message = message ;
}

/*

    CR (carriage return), CRLF (CR LF) LF (line feed)

    typically parse the request-line into its component parts
    by splitting on whitespace . since no whitespace is allowed in the three components

    -- ERROR--
    Recipients of an invalid request-line SHOULD respond with either a
    400 (Bad Request) error or a 301 (Moved Permanently)

*/

void Request::parseStartLine(const std::string & line) 
{
 
    std::vector<std::string> tokens ;
    
    tokens = split(line) ;
    std::vector<std::string>::iterator start = tokens.begin();
    // std::vector<std::string>::iterator end = tokens.end();

    // start line should be 3 elements <HTTP Method>  <Request-URI>  <HTTP Version>
    if ( tokens.size() != 3 )
        throw 400 ;
        
    //  <HTTP Method>
    if ( ! this->Valide_method(*start) )
        throw 400 ;
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



void Request::parseMessage()
{
    std::string line ;
    std::istringstream stream(message) ; // Treat the string as an input stream 

    getline(stream , line) ; // get start line

    if ( stream.eof() )
        throw 400 ; // error : 400 (Bad Request)

    parseStartLine(line) ;
    
    while ( getline( stream , line )  )
    {
        if ( line.length() == 1 && line[0] == '\r' ) // CRLF
            break ;
            
        if ( stream.eof() ) // if not ending with CRLF
            throw 400 ; 

        parseHeader( line ) ;

        // std::cout << line << std::endl ;
    }


    if (this->headers.find("Host") == this->headers.end() )
        throw 400 ;

    
}

bool Request::Valide_method( const std::string methodName )
{
    std::string methods[3] = { "GET" , "POST" , "HEAD" } ;
    
    for ( size_t i = 0 ; i < methods->length() ; i++ )
        if ( methods[i] == methodName )
            return ( true ) ;
            
    return ( false ) ;
}






Request::~Request() 
{

}