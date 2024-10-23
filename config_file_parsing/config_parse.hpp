/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 15:09:28 by ymafaman          #+#    #+#             */
/*   Updated: 2024/10/23 10:54:55 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSE_HPP
# define CONFIG_PARSE_HPP

#include "HttpContext.hpp"
#include "ServerContext.hpp"
#include "LocationContext.hpp"
#include "../helper_functions.hpp"

#include <iostream>
#include <fstream>
#include <exception>
#include <queue>
#include <string>
#include <sstream>
#include <algorithm>

typedef struct token_info
{
    std::string     token;
    unsigned int    line_num;
    bool            is_sep; // This indecates if the token is a separator or not. Separators are (";" and "{" and "}")
} token_info;
void    config_tokenizer(std::string file_name, HttpContext & http_config) ;
// void    config_tokenizer(std::string file_name);
void    append_token_to_queue(std::string token, std::queue<std::string>& tokens_queue, std::string file_name, unsigned int line_num);
void    store_config(HttpContext& http_config, std::queue<token_info>& tokens_queue, std::string file_name, std::string context);
bool    is_http_ctx_dir(std::string directive);
bool    is_server_ctx_dir(std::string directive);
bool    is_location_ctx_dir(std::string directive);
void    throw_config_parse_exception(std::string type, std::string token, std::string file_name, unsigned int line_num);
void    throw_wrong_value_exception(std::string directive, std::string wrong_value, std::string file_name, unsigned int line_num);

std::string extract_cgi_extension(std::queue<token_info>& tokens_queue, std::string file_name);
std::pair <unsigned short, std::string> extract_error_page_info(std::queue<token_info>& tokens_queue, std::string file_name);
bool    extract_autoindex_value(std::queue<token_info>& tokens_queue, std::string file_name);
std::vector<unsigned short>  extract_port_number(std::queue<token_info>& tokens_queue, std::string file_name);
std::string    extract_root_dir(std::queue<token_info>& tokens_queue, std::string file_name);
std::string extract_upload_dir(std::queue<token_info>& tokens_queue, std::string file_name);
std::string extract_index(std::queue<token_info>& tokens_queue, std::string file_name);
std::vector<std::string>    extract_srv_names(std::queue<token_info>& tokens_queue, std::string file_name);
std::vector<std::string>    extract_allowed_methods(std::queue<token_info>& tokens_queue, std::string file_name);
std::pair <unsigned short, std::string> extract_redirection_info(std::queue<token_info>& tokens_queue, std::string file_name);


#endif