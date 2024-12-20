/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_functions.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 21:38:05 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/03 10:27:24 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "utils.hpp"

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

void	*ft_memset(void *b, int c, size_t len)
{
	unsigned int	i;

	i = 0;
	while (i < len)
	{
		*((unsigned char *)(b) + i) = (unsigned char) c;
		i++;
	}
	return (b);
}

void    err_throw( const char * message )
{
    throw (std::string("Webserv : ") + message).c_str();
}

