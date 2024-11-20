/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_functions.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 21:38:05 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/08 20:24:10 by ymafaman         ###   ########.fr       */
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

void    close_sockets_on_error(std::vector<struct ListenerSocket>& active_socks)
{
    std::vector<struct ListenerSocket>::iterator it = active_socks.begin();
    std::vector<struct ListenerSocket>::iterator end = active_socks.end();

    for ( ; it != end; it++ )
    {
        close(it->get_sock_fd());   
    }
}

void    close_client_sockets_on_error(std::vector<struct ClientSocket *>& clients_socks)
{
    std::vector<struct ClientSocket *>::iterator it = clients_socks.begin();
    std::vector<struct ClientSocket *>::iterator end = clients_socks.end();

    for ( ; it != end; it++ )
    {
        close((*it)->get_sock_fd());
        delete (*it)->request;
        delete *it;   
    }
}