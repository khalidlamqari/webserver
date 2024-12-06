/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 14:16:59 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/06 17:24:22 by klamqari         ###   ########.fr       */
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
std::string get_rand_file_name(size_t & file_num)
{
    std::ostringstream file_name;
    std::string path = "/tmp/.web_server_response_file_";

    file_name << path;
    file_name << file_num;

    if (access(file_name.str().c_str(), F_OK) == 0)
    {
        file_num += 1;
        return get_rand_file_name(file_num);
    }
    else
    {
        return file_name.str();
    }
}

// echo -e "GET /about.html HTTP/1.1\r\nHost: google.com\r\nConnection: close\r\n\r\n" | nc 127.0.0.1 5000