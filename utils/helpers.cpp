/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 14:16:59 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/16 10:53:09 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/main.hpp"

bool is_all_WS( const std::string & str )
{
    for ( size_t i = 0; i < str.length() ; i++ )
        if ( ! std::iswspace(str[i]) )
            return ( false ) ;
    return ( true ) ;
}

bool is_all_digit(const std::string & str)
{
    for ( size_t i = 0; i < str.length() ; i++ )
        if ( ! std::isdigit( str[i]) )
            return ( false ) ;
    return ( true ) ;
}

// echo -e "GET /about.html HTTP/1.1\r\nHost: google.com\r\nConnection: close\r\n\r\n" | nc 127.0.0.1 5000