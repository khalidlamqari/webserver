/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_values_extracter.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/12 15:44:35 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/01 18:56:14 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

bool    extract_autoindex_value(std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::string token;
    bool        value;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
    else if (token != "on" && token != "off")
        throw_wrong_value_exception("autoindex", token, file_name, tokens_queue.front().line_num);
    else if (token == "on")
        value = true;
    else
        value = false;

    tokens_queue.pop();
    token = tokens_queue.front().token;
    
    if (token == ";" && tokens_queue.front().is_sep)
    {
        tokens_queue.pop();
        return value;
    }
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "autoindex", file_name, tokens_queue.front().line_num);
    else
        throw_wrong_value_exception("wrong args num", "autoindex", file_name, tokens_queue.front().line_num);

    return value; 
}

std::pair <unsigned short, std::string> extract_error_page_info(std::queue<token_info>& tokens_queue, std::string file_name)
{
    unsigned short  error_code;
    std::string     token;
    std::string     path;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
    else if (!is_all_digits(token) || token.length() != 3 || token < "300" || token > "599")
        throw_wrong_value_exception("error_page", token, file_name, tokens_queue.front().line_num);
    else
        error_code = std::stoi(token);

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if (token == ";" && tokens_queue.front().is_sep)
        throw_wrong_value_exception("wrong args num", "error_page", file_name, tokens_queue.front().line_num);
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "error_page", file_name, tokens_queue.front().line_num);
    else
        path = token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if (token == ";" && tokens_queue.front().is_sep)
    {
        tokens_queue.pop();
        return std::make_pair(error_code, path);
    }
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "autoindex", file_name, tokens_queue.front().line_num);
    else
        throw_wrong_value_exception("wrong args num", "autoindex", file_name, tokens_queue.front().line_num);

    return std::make_pair(error_code, token);  
}

std::string extract_cgi_extension(std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::string cgi_extension;
    std::string token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
    else if (token != ".php")
        throw_wrong_value_exception("cgi_extension", token, file_name, tokens_queue.front().line_num);
    else
        cgi_extension = token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if (token == ";" && tokens_queue.front().is_sep)
    {
        tokens_queue.pop();
        return cgi_extension;
    }
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "cgi_extention", file_name, tokens_queue.front().line_num);
    else
        throw_wrong_value_exception("wrong args num", "cgi_extention", file_name, tokens_queue.front().line_num);

    return cgi_extension;  
}

std::vector<unsigned short>  extract_port_number(std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::vector<unsigned short>  port_nums;
    std::string                  token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
    
    
    while (!tokens_queue.empty() && token != ";" && token != "{" && token != "}")
    {
        if (!is_all_digits(token) || (token.length() != 4 && token.length() != 5) || (token.length() == 4 && token < "1024" ) || (token.length() == 5 && token > "49151"))
            throw_wrong_value_exception("listen", token, file_name, tokens_queue.front().line_num);
        else
        {
            port_nums.push_back(std::stoi(token));
            tokens_queue.pop();
            token = tokens_queue.front().token;
        }
    }

    if (tokens_queue.empty())
        throw_config_parse_exception("unterminated", "listen", file_name, tokens_queue.front().line_num);

    if (token == ";" && tokens_queue.front().is_sep)
        tokens_queue.pop();
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "listen", file_name, tokens_queue.front().line_num);
    else
        throw_wrong_value_exception("wrong args num", "listen", file_name, tokens_queue.front().line_num);

    return port_nums;  
}

std::string    extract_root_dir(std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::string root;
    std::string token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
    else
        root = token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if (token == ";" && tokens_queue.front().is_sep)
    {
        tokens_queue.pop();
        return root;
    }
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "root", file_name, tokens_queue.front().line_num);
    else
        throw_wrong_value_exception("wrong args num", "root", file_name, tokens_queue.front().line_num);

    return root;  
}

std::string extract_upload_dir(std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::string directory;
    std::string token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
    else
        directory = token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if (token == ";" && tokens_queue.front().is_sep)
    {
        tokens_queue.pop();
        return directory;
    }
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "upload_dir", file_name, tokens_queue.front().line_num);
    else
        throw_wrong_value_exception("wrong args num", "upload_dir", file_name, tokens_queue.front().line_num);

    return directory;  
}

std::string extract_index(std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::string index;
    std::string token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
    else
        index = token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if (token == ";" && tokens_queue.front().is_sep)
    {
        tokens_queue.pop();
        return index;
    }
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "index", file_name, tokens_queue.front().line_num);
    else
    {
        throw_config_parse_exception("wrong args num", "index", file_name, tokens_queue.front().line_num);
    }

    return index;  
}

std::vector<std::string>    extract_srv_names(std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::vector<std::string>    names;
    std::string                 token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);

    while (!tokens_queue.empty() && token != ";" && token != "{" && token != "}")
    {
        names.push_back(token);
        tokens_queue.pop();
        token = tokens_queue.front().token;
    }

    if (tokens_queue.empty())
        throw_config_parse_exception("unterminated", "server_names", file_name, tokens_queue.front().line_num);

    if (token == ";" && tokens_queue.front().is_sep)
    {
        tokens_queue.pop();
    }
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "server_names", file_name, tokens_queue.front().line_num);

    return names; 
}

size_t    extract_max_body_size(std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::string     token;
    size_t          size = 0;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
    else if (!is_all_digits(token) || token.length() > 11 || (token.length() == 11 && token > "12500000000"))
        throw_wrong_value_exception("max_body_size", token, file_name, tokens_queue.front().line_num);
    else
        size = std::stoul(token);
    
    tokens_queue.pop();
    token = tokens_queue.front().token;

    if (token == ";" && tokens_queue.front().is_sep)
    {
        tokens_queue.pop();
    }
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "server_names", file_name, tokens_queue.front().line_num);

    return size;
}

std::vector<std::string>    extract_allowed_methods(std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::vector<std::string>    methods;
    std::string                 token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);

    while (!tokens_queue.empty() && token != ";" && token != "{" && token != "}")
    {
        if (token != "GET" && token != "POST" && token != "DELETE" && token != "HEAD")
            throw_wrong_value_exception("allowed_methods", token, file_name, tokens_queue.front().line_num);
        else
        {
            methods.push_back(token);
            tokens_queue.pop();
            token = tokens_queue.front().token;
        }
    }

    if (tokens_queue.empty())
        throw_config_parse_exception("unterminated", "allowed_methods", file_name, tokens_queue.front().line_num);

    if (token == ";" && tokens_queue.front().is_sep)
    {
        tokens_queue.pop();
    }
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "allowed_methods", file_name, tokens_queue.front().line_num);

    return methods; 
}

std::pair <unsigned short, std::string> extract_redirection_info(std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::pair <unsigned short, std::string> redirection_info;
    std::string                             token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
    else if (!is_all_digits(token) || token.length() > 3)
        throw_wrong_value_exception("return", token, file_name, tokens_queue.front().line_num);
    else
        redirection_info.first = std::stoi(token);

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if (token == ";" && tokens_queue.front().is_sep)
        redirection_info.second = "";
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "return", file_name, tokens_queue.front().line_num);
    else
        redirection_info.second = token;

    if (redirection_info.second != "") // pop only if a string is provided with the code.
        tokens_queue.pop();
    token = tokens_queue.front().token;

    if (token == ";" && tokens_queue.front().is_sep)
    {
        tokens_queue.pop();
        return redirection_info;
    }
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "return", file_name, tokens_queue.front().line_num);
    else
        throw_wrong_value_exception("wrong args num", "return", file_name, tokens_queue.front().line_num);

    return redirection_info; 
}

std::string extract_host_name(std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::string token;
    std::string host;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
    else
        host = token;

    tokens_queue.pop();
    token = tokens_queue.front().token;

    if (token == ";" && tokens_queue.front().is_sep)
    {
        tokens_queue.pop();
        return host;
    }
    else if ((token == "{" || token == "}") && tokens_queue.front().is_sep)
        throw_config_parse_exception("unterminated", "host", file_name, tokens_queue.front().line_num);
    else
        throw_wrong_value_exception("wrong args num", "host", file_name, tokens_queue.front().line_num);

    return host;  
}