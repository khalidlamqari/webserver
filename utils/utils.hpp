/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 21:45:38 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/21 11:10:12 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

// #include "../webserv.hpp"
#include <map>

#include <vector>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <map>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include "../response/Response.hpp"

class Response;

bool    is_space(char c);
void    err_throw( const char * message );
bool    is_all_digits(const std::string& str);
void	*ft_memset(void *b, int c, size_t len);


std::string         get_rand_file_name(size_t & file_num);
bool                normalize_target(std::string &target);
std::string         get_content_type( const std::string & file_name );
const std::string   get_time(time_t sec);
bool                is_dir( const std::string & path );
bool                is_file(std::string path);
void                set_connection(const std::map<std::string , std::string> & headers, std::string & connection );
bool                is_existe(const std::string & path);
bool                check_is_cgi(std::string & path, std::string & cgi_extention, bool is_bad_request);
void                create_socket_pair(Response & response);

#endif