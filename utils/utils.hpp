/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 21:45:38 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/25 12:06:06 by klamqari         ###   ########.fr       */
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
class ClientSocket;

bool    is_space(char c);
void    err_throw( const char * message );
bool    is_all_digits(const std::string& str);
void	*ft_memset(void *b, int c, size_t len);


std::string         get_rand_file_name(size_t & file_num);
void                normalize_target(std::string &target, unsigned short & status);
std::string         get_content_type( const std::string & file_name );
const std::string   get_time(time_t sec);
bool                is_dir( const std::string & path );
bool                is_file(std::string path);
bool                is_existe(const std::string & path);
bool                check_is_cgi(const std::string & path, const std::string & cgi_extention, bool is_bad_request) ;
void                create_socket_pair(Response & response);



void    create_html_table(std::string & ls_files, const std::string & target);
void    append_row( std::string  path , std::string target, struct dirent * f, std::string & ls_files );
void    set_cgi_requerements( Response & response, bool & is_cgi);
const ServerContext * get_server_context(ClientSocket & clientsocket);
void extract_info_from_location(Response & response, LocationContext & location);
void extract_info_from_server(Response & response,  const ServerContext & servercontext);
void  remove_last_slash( std::string & target );



#endif



/* 

delete folder OK


*/