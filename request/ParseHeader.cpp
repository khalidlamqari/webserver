/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseHeader.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 17:26:40 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/11 12:20:37 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# include "Request.hpp"

/*

    header-field = field-name ":" OWS field-value OWS
    OWS = (optional whitespace),

*/

void Request::parseHeader(const std::string & line ) 
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

    // check the field name already exists in map
    if ( this->headers.find(field_name) != this->headers.end() )
        throw 400;

    this->headers[field_name] = field_value ;
}
