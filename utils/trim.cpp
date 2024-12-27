/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trim.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 10:18:07 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/27 09:53:42 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "../includes/main.hpp"

// std::string trim(const std::string & str )
// {
//     size_t start, end ;

//     start    = 0;
//     end      = str.length() - 1;

//     while ( start < str.length() && std::iswspace(str[ start ]) ) // skip whitespaces from the begining
//         start++ ;

//     while ( end >= 0 && std::iswspace(str[ end ]) )   // skip whitespaces from the end
//         end--;
//     end++;

//     if ( start >= end )    // if all whitespaces
//         return ( "" );

//     return ( str.substr(start, end - start) );
// }
