/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseHeader.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 17:26:40 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/11 19:12:01 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# include "Request.hpp"

/*

    header-field = field-name ":" OWS field-value OWS
    OWS = (optional whitespace),

*/

void Request::parseHeader( const std::string & line )
{
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

    this->headers[field_name] = values ;
}

bool isdelimiter( char c )
{
    std::string delimiters = ",:;" ;
    if (delimiters.find(c) == std::string::npos )
        return false ;
    return true ;
}

std::vector<std::string> Request::parse_header_values( const std::string & field_value )
{
    std::vector<std::string> strings ;


    size_t i = 0 ;
    size_t start , end;
    
    while ( i < field_value.length() )
    {
        while ( i < field_value.length() && (std::iswspace(field_value[i]) || isdelimiter(field_value[i]) ) ) 
            i++ ;
        
        start = i ;

        while ( i < field_value.length() && ! (std::iswspace(field_value[i]) || isdelimiter(field_value[i])) ) 
            i++;

        end = i;

        if (start == end)
            break ;

        strings.push_back( field_value.substr( start, end - start ) );
    }
    return ( strings );

}
