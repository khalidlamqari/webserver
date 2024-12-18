/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 21:45:38 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/10 11:48:36 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include "../webserv.hpp"

bool                is_space(char c);
void                err_throw( const char * message );
bool                is_all_digits(const std::string& str);
void	            *ft_memset(void *b, int c, size_t len);
void                close_sockets_on_error(std::vector<struct ListenerSocket>& active_socks);
void                close_client_sockets_on_error(std::vector<struct ClientSocket *>& clients_socks);
std::string         get_rand_file_name(size_t & file_num);
bool                normalize_target(std::string &target);
std::string         get_content_type( const std::string & file_name );
const std::string   get_time(time_t sec);
bool                is_dir( const std::string & path );
bool                is_file(std::string path);
void                set_connection(std::map<std::string , std::string> & headers, std::string & connection );
bool                is_existe(const std::string & path);

#endif