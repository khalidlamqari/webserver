/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_functions.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 21:38:05 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/10 20:06:12 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "helper_functions.hpp"

bool is_all_digits(const std::string& str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!std::isdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

bool    is_space(char c)
{
    return (c == 32 || (c >= 9 && c <= 13));
}
