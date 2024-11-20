/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 21:45:38 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/04 14:00:51 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include "../webserv.hpp"

bool    is_space(char c);
void    err_throw( const char * message );
bool    is_all_digits(const std::string& str);
void	*ft_memset(void *b, int c, size_t len);
void    close_sockets_on_error(std::vector<struct ListenerSocket>& active_socks);
void    close_client_sockets_on_error(std::vector<struct ClientSocket *>& clients_socks);


#endif