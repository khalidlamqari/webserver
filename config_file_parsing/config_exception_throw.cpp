/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_exception_throw.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 17:08:12 by ymafaman          #+#    #+#             */
/*   Updated: 2024/10/14 18:17:21 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_parse.hpp"

void    throw_config_parse_exception(std::string type, std::string token, std::string file_name, unsigned int line_num)
{
    std::ostringstream  error_info;
    
    if (type == "Unexpected")
    {
        error_info << "webserv : unexpected \"" + token + "\" in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (type == "Not allowed")
    {
        error_info << "webserv : \"" + token + "\" directive is not allowed here in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (type == "Unknown")
    {
        error_info << "webserv : unknown directive \"" + token + "\"in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (type == "unclosed_ctx")
    {
        error_info << "webserv : unexpected end of file, expecting \"}\" in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (type == "duplication")
    {
        error_info << "webserv : \"" + token + "\" directive is duplicate in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (type == "unterminated")
    {
        error_info << "webserv : directive \"" + token + "\" is not terminated by \";\" in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (type == "wrong args num")
    {
        error_info << "webserv : invalid number of arguments in \"" + token + "\" directive in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (type == "EOF")
    {
        error_info << "webserv : unexpected end of file, expecting \";\" or \"}\" in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (type == "no_openning")
    { 
        error_info << "webserv : directive " + token + " has no opening \"{\" in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
}

void    throw_wrong_value_exception(std::string directive, std::string wrong_value, std::string file_name, unsigned int line_num)
{
    std::ostringstream  error_info;

    if (directive == "autoindex")
    {
        error_info << "webserv : invalid value \"" + wrong_value + "\" in \"autoindex\" directive,  it must be \"on\" or \"off\" in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (directive == "error_page")
    {
        error_info << "webserv : value \"" + wrong_value + "\" must be a number between 300 and 599 in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (directive == "cgi_extension")
    {
        error_info << "webserv : invalid value \"" + wrong_value + "\" in \"cgi_extension\" directive, it must be \".php\" in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (directive == "listen")
    {
        error_info << "webserv : value \"" + wrong_value + "\" must be a number between 1024 and 49151 in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (directive == "allowed_methods")
    {
        error_info << "webserv : invalid method \"" + wrong_value + "\" in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
    else if (directive == "return")
    {
        error_info << "webserv : invalid return code \"" + wrong_value + "\" in " + file_name + ":";
        error_info <<  line_num;
        throw std::invalid_argument(error_info.str());
    }
}