/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 13:04:29 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/31 18:13:43 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/main.hpp"

std::vector<std::string> split(const std::string & str)
{
    std::vector<std::string> strings ;


    size_t i = 0 ;
    size_t start , end;
    
    while ( i < str.length() )
    {
        while ( i < str.length() && std::iswspace(str[i]) ) 
            i++ ;
        
        start = i ;
        
        while ( i < str.length() && ! std::iswspace(str[i]) ) 
            i++;

        end = i;

        if (start == end)
            break ;
        
        strings.push_back( str.substr( start, end - start ) );
    }
    return ( strings );
}

std::vector<std::string> _split_(const std::string & str, char c)
{
    std::vector<std::string> strings ;


    size_t i = 0 ;
    size_t start , end;
    
    while ( i < str.length() )
    {
        while ( i < str.length() && str[i] == c ) 
            i++ ;
        
        start = i ;
        
        while ( i < str.length() && str[i] != c ) 
            i++;

        end = i;

        if (start == end)
            break ;
        
        strings.push_back( str.substr( start, end - start ) );
    }
    return ( strings );
}