/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 11:06:28 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/18 11:34:58 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request( void )
{
    start_line_is_parsed = false;
    headers_parsed = false;
    body_is_parsed = false;
    is_chunked = false;
    is_persistent = true;
    is_bad = false;
    is_multipart = false;
    content_length_is_set = false;
    host_is_set = false;
    content_length = 0;
    content_type = "text/plain";
    first_part_reached = false;
    last_part_reached = false;
    has_incomplete_part = false;
    undone_chunk = false;     // useless ?
    size_left = 0;
    total_chunks_length = 0;
    is_ready = false;
    first_chunk_fixed = false;
}

Request::~Request()
{
    
}

/* Getters */

const std::map<std::string, std::string> & Request::get_headers() const
{
    return (this->headers);
}

size_t      Request::get_total_chunks_length()
{
    return this->total_chunks_length;
}

bool    Request::hasParsedStartLine()
{
    return this->start_line_is_parsed;
}

bool    Request::hasParsedHeaders()
{
    return this->headers_parsed;
}

bool    Request::hasParsedBody()
{
    return this->body_is_parsed;
}

bool    Request::hostIsSet()
{
    return this->host_is_set;
}

bool    Request::hasReachedFirstPart( void )
{
    return first_part_reached;
}

bool    Request::hasReachedLastPart( void )
{
    return last_part_reached;
}

bool    Request::isBadRequest()
{
    return this->is_bad;
}

bool    Request::ContentLengthIsSet()
{
    return this->content_length_is_set;
}

bool    Request::isReady()
{
    return this->is_ready;
}

bool    Request::isChunked()
{
    return this->is_chunked;
}

bool    Request::isPersistent()
{
    return this->is_persistent;
}

bool    Request::isMultipart()
{
    return this->is_multipart;
}

std::string Request::get_target()
{
    return this->target;
}

std::string Request::get_query()
{
    return this->query;
}

std::string Request::get_method()
{
    return this->method;
}

std::string Request::get_version()
{
    return this->version;
}

std::string Request::get_body()
{
    return this->body;
}

std::string Request::getUnparsedMsg()
{
    return this->unparsed_msg;
}

size_t      Request::getContentLength()
{
    return this->content_length;
}

std::string Request::get_boundary()
{
    return this->boundary;
}

bool    Request::hasIncompletePart()
{
    return this->has_incomplete_part;   
}


t_part &	Request::get_latest_part()
{
    if (!parts.size() || parts.back().is_complete)
    {
        t_part  new_part;
        new_part.is_complete = false;
        new_part.is_new = true;
        new_part.header_parsed = false;
        new_part.content_type = "text/plain";
        parts.push_back(new_part);
        std::cout << "created !" << std::endl;
    }
    return this->parts.back();
}

size_t    Request::hasAnUndoneChunk( void )
{
    return this->size_left;
}

/* Setters */

void      Request::set_total_chunks_length(size_t total_length)
{
    this->total_chunks_length += total_length;
}

void	Request::set_new_part( t_part & new_part )
{
    this->parts.push_back(new_part);
}

void    Request::set_method( const std::string& method )
{
    this->method = method;
}

void    Request::set_version( const std::string& version )
{
    if (version == "HTTP/1.0")
        this->is_persistent = false;

    this->version = version;
}

void    Request::set_target( const std::string& target )
{
    this->target = target;
}

void    Request::set_query( std::string query )
{
    this->query = query;
}

void	Request::set_content_length( const size_t & length )
{
    this->content_length = length;
    this->content_length_is_set = true;
}

void    Request::set_boundary( const std::string & boundary )
{
    this->boundary = boundary;
}


void    Request::markStartLineParsed( const bool& parsed )
{
    this->start_line_is_parsed = parsed;
}

void    Request::markHeadersParsed( const bool & parsed )
{
    this->headers_parsed = parsed;
}

void    Request::markBodyParsed( const bool & parsed )
{
    this->body_is_parsed = parsed;
}

void    Request::markAsBad( int i )
{
    // Testing
    std::cerr << i << std::endl;
    this->is_bad = true;
    throw "Bad request!";
}

void    Request::markAsChunked()
{
    this->is_chunked = true;
}

void    Request::markAsReady( const bool & ready )
{
    this->is_ready = ready;
}

void    Request::markAsPersistent( const bool& persist )
{
    this->is_persistent = persist;
}

void    Request::markContentLengthAsSet()
{
    this->content_length_is_set = true;
}

void    Request::markAsMultipart()
{
    this->is_multipart = true;
}

void    Request::markHostAsSet()
{
    this->host_is_set = true;
}

void    Request::markFirstPartAsReached()
{
    this->first_part_reached = true;
}

void    Request::markLastPartAsReached()
{
    this->last_part_reached = true;
}

void    Request::set_parsingErrorCode( short code )
{
    this->parsingErrorCode = code;
}

void    Request::storeUnparsedMsg(const std::string & msg )
{
    this->unparsed_msg.append(msg);
}

void    Request::setHasIncompletePart( bool & incomplete )
{
    this->has_incomplete_part = incomplete;
}

void    Request::markAsHasUndoneChunk( bool undone, size_t size_left )
{
    if (undone)
        this->size_left = size_left;
    else
        this->size_left = 0;
    this->undone_chunk = undone;
}

/* Methods */

void    Request::resetUnparsedMsg()
{
    this->unparsed_msg.clear();
}

void    Request::setHeader( const std::string& name, const std::string& value )
{   
    if (name == "TRANSFER-ENCODING" && value == "chunked")
        this->markAsChunked();    

    this->headers[name] = value;
}

void    Request::set_body( const std::string & body )
{
    this->body += body;
}

std::string    Request::build_boundary(int type)
{
    std::string _boundary = this->boundary;

    _boundary.insert(0, "--");

    if (type == 2)
        _boundary.append("--");
    if (type == 3)
        _boundary.insert(0, "\r\n");

    _boundary.append("\r\n");

    return _boundary;
}


// Testing

void    Request::print_headrs()
{
    std::cout << "Headers : " << std::endl;
    std::map<std::string, std::string>::iterator it = this->headers.begin();
    std::map<std::string, std::string>::iterator end = this->headers.end();
    for ( ; it != end; it++)
        std::cout << (*it).first << ": " << (*it).second << std::endl;
}


#include <fstream>
void    Request::print_files()
{

    std::ofstream image("image.png");
        
        // image.open();
    for (std::vector<t_part>::iterator i = parts.begin(); i != parts.end(); i++)
    {
        // std::cout << "{"  ;
        image << i->file_content;
        image.flush();
        image.close();
        // std::cout << "}" << std::endl;      
    }
}

void    Request::drop_last_part()
{
    this->parts.pop_back();
}