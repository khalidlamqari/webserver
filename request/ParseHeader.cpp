/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseHeader.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 17:26:40 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/16 11:54:00 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# include "Request.hpp"

bool isdelimiter( char c ) ;
char get_delimiter(const std::string & str, size_t pos) ;
size_t get_end_of_token(char delimiter , size_t i , const std::string & field_value ) ;

/*
*   header-field = field-name ":" OWS field-value OWS
*   OWS = (optional whitespace),
*   
*   A sender MUST NOT send a Content-Length header field in any message
*   that contains a Transfer-Encoding header field.
*
*
*/

void Request::parseHeader( const std::string & line )
{
    if ( line[ line.length() - 1 ] != '\r' )
        throw 400 ;
    size_t i =  line.find( ":" );
    if (i == std::string::npos)
        throw 400 ;
 
    std::string field_name = line.substr( 0, i ); // get field name

    // check if any whitespace between field name and ':'
    if (field_name.length() && std::iswspace( field_name[ field_name.length() - 1 ] ))
        throw 400 ;

                                    // get field value
    std::string field_value = trim( line.substr( i + 1) ); // remove OWS
    std::vector<std::string> values = this->parse_header_values(field_value) ;
    // check the field name already exists in map
    if ( this->headers.find(field_name) != this->headers.end() )
        throw 400 ;
    
    if ( field_name == "Content-Length" )
    {
        if ( this->headers.find("Transfer-Encoding") != this->headers.end() )
            throw 400 ;
    }
    else if ( field_name == "Transfer-Encoding") 
    {
        if ( this->headers.find("Content-Length") != this->headers.end() )
            throw 400 ;
    }
    this->headers[field_name] = values ;
}

std::vector<std::string> Request::parse_header_values( const std::string & field_value )
{
    std::vector<std::string> strings ;

    size_t i = 0 ;
    size_t start , end ;
    char delimiter ;

    while ( i < field_value.length() )
    {
        while ( i < field_value.length() && (std::iswspace(field_value[i]) || isdelimiter(field_value[i]) ) ) 
            i++ ;
        delimiter = get_delimiter( field_value, i ) ;
        start = i ;
        end = get_end_of_token(delimiter, i , field_value);
        i = end ;

        if (start >= end)
            break ;

        strings.push_back( field_value.substr( start, end - start ) );
        i++ ;
    }
    return ( strings );
}

bool isdelimiter( char c )
{
    std::string delimiters = ",;" ;
    if (delimiters.find(c) == std::string::npos )
        return false ;
    return true ;
}

char get_delimiter(const std::string & str, size_t pos)
{
    while ( pos < str.length() )
    {
        if ( str[pos] == '\\' )
        {
            pos++ ;
            continue ;
        }
            
        if (str[pos] == '"' || str[pos] == '\'' || str[pos] == '(')
            return str[pos] ;
        pos++ ;
    }
    return ( ' ' ) ;
}

size_t get_end_of_token(char delimiter , size_t i , const std::string & field_value )
{
    int count = 0 ;
    
    if ( std::iswspace(delimiter) || delimiter == '\0')
    {
        while ( i < field_value.length() && ! (std::iswspace(field_value[i]) || isdelimiter(field_value[i])) )
        {
            if ( field_value[i] == '\\' )
            {
                i++ ;
                continue ;
            }
            i++ ;
        }
        return i ;
    }
    else if (delimiter == '(')
    {
        delimiter = ')' ;
        while ( i < field_value.length() &&  field_value[i] != delimiter )
        {
            if ( field_value[i] == '\\' )
            {
                i++ ;
                continue ;
            }
            i++ ;
        }

        if ( field_value[i] != delimiter )
            throw 400 ;
        return i + 1;
    }
    else
    {
        while ( i < field_value.length() && count != 2)
        {
            if ( field_value[i] == '\\' )
            {
                i++ ;
                continue ;
            }
            if (delimiter == field_value[i])
                count++ ;
            i++ ;
        }
        if ( field_value[i - 1] != delimiter )
            throw 400 ;

        return i ;
    }
        
}
