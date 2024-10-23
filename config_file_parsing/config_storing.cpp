/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_storing.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 14:47:39 by ymafaman          #+#    #+#             */
/*   Updated: 2024/10/23 10:34:57 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_parse.hpp"

void    store_http_directives(HttpContext& http_config, std::queue<token_info>& tokens_queue, std::string file_name)
{
    /* The following flags are used to track duplicate directives */
    static bool    cgi_ext_is_set;
    static bool    auto_ind_is_set;

    std::string token = tokens_queue.front().token;
    if (!is_http_ctx_dir(token))
    {
        // TODO : The following lines are repeated multiple times, create a function to do this repetetive job;
        if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
            throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
        else if (is_server_ctx_dir(token) || is_location_ctx_dir(token))
            throw_config_parse_exception("Not allowed", token, file_name, tokens_queue.front().line_num);
        else
            throw_config_parse_exception("Unknown", token, file_name, tokens_queue.front().line_num); 
    }
    else if (token == "server")
    {
        tokens_queue.pop();
        store_config(http_config, tokens_queue, file_name, "server");
    }
    else if (token == "autoindex")
    {
        if (auto_ind_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);

        http_config.set_auto_index(extract_autoindex_value(tokens_queue, file_name));

        auto_ind_is_set = true;
    }
    else if (token == "error_page")
    {
        http_config.set_error_page(extract_error_page_info(tokens_queue, file_name));
    }
    else if (token == "cgi_extention")
    {
        if (cgi_ext_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);

        http_config.set_cgi_extension(extract_cgi_extension(tokens_queue, file_name));

        cgi_ext_is_set = true;
    }
}

void    store_serv_directives(HttpContext& http_config, std::queue<token_info>& tokens_queue, std::string file_name)
{
    ServerContext&   server = http_config.get_latest_server();

    std::string token = tokens_queue.front().token;

    if (!is_server_ctx_dir(token))
    {
        if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
            throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
        else if (is_http_ctx_dir(token) || is_location_ctx_dir(token))
            throw_config_parse_exception("Not allowed", token, file_name, tokens_queue.front().line_num);
        else
            throw_config_parse_exception("Unknown", token, file_name, tokens_queue.front().line_num);
    }
    else if (token == "location")
    {
        /* Creating a new locationContext object in the ServerLocation::locations vector */
        http_config.get_latest_server().set_new_location();

        tokens_queue.pop();
        token = tokens_queue.front().token;

        /* Cheking if the location is exact or not */
        if (token == "=")
        {
            http_config.get_latest_server().get_latest_location().make_exact();
            tokens_queue.pop();
        }

        /* Getting the location's relative path */
        token = tokens_queue.front().token;

        if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
            throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
        else
            http_config.get_latest_server().get_latest_location().set_location(token);

        tokens_queue.pop(); // now the current token should be a "{";

        store_config(http_config, tokens_queue, file_name, "location");
    }
    else if (token == "autoindex")
    {
        if (server.auto_ind_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);

        server.set_auto_index(extract_autoindex_value(tokens_queue, file_name));

        server.auto_ind_is_set = true;
    }
    else if (token == "error_page")
    {
        server.set_error_page(extract_error_page_info(tokens_queue, file_name));
    }
    else if (token == "cgi_extention")
    {
        if (server.cgi_ext_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);

        server.set_cgi_extension(extract_cgi_extension(tokens_queue, file_name));

        server.cgi_ext_is_set = true;
    }
    else if (token == "listen")
    {
        if (server.port_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);
        
        server.set_ports(extract_port_number(tokens_queue, file_name));

        server.port_is_set = true;
    }
    else if (token == "root")
    {
        if (server.root_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);
        
        server.set_root_directory(extract_root_dir(tokens_queue, file_name));

        server.root_is_set = true;
    }
    else if (token == "upload_dir")
    {
        if (server.upl_dir_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);
        
        server.set_upload_dir(extract_upload_dir(tokens_queue, file_name));

        server.upl_dir_is_set = true;
    }
    else if (token == "index")
    {
        if (server.index_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);
        
        server.set_upload_dir(extract_index(tokens_queue, file_name));

        server.index_is_set = true;
    }
    else if (token == "server_names")
    {
        if (server.srv_names_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);
        
        server.set_server_names(extract_srv_names(tokens_queue, file_name));

        server.srv_names_is_set = true;
    }
    else if (token == "allowed_methods")
    {
        if (server.methods_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);

        server.set_allowed_methods(extract_allowed_methods(tokens_queue, file_name));

        server.methods_is_set = true;
    }
}

void    store_location_directives(LocationContext& location, std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::string token = tokens_queue.front().token;   

    if (!is_location_ctx_dir(token))
    {
        if ((token == ";" || token == "{" || token == "}") && tokens_queue.front().is_sep)
            throw_config_parse_exception("Unexpected", token, file_name, tokens_queue.front().line_num);
        else if (is_http_ctx_dir(token) || is_location_ctx_dir(token))
            throw_config_parse_exception("Not allowed", token, file_name, tokens_queue.front().line_num);
        else
            throw_config_parse_exception("Unknown", token, file_name, tokens_queue.front().line_num);
    }
    else if (token == "autoindex")
    {
        if (location.auto_ind_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);

        location.set_auto_index(extract_autoindex_value(tokens_queue, file_name));

        location.auto_ind_is_set = true;
    }
    else if (token == "error_page")
    {
        location.set_error_page(extract_error_page_info(tokens_queue, file_name));
    }
    else if (token == "cgi_extention")
    {
        if (location.cgi_ext_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);

        location.set_cgi_extension(extract_cgi_extension(tokens_queue, file_name));

        location.cgi_ext_is_set = true;
    }
     else if (token == "root")
    {
        if (location.root_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);
        
        location.set_root_directory(extract_root_dir(tokens_queue, file_name));

        location.root_is_set = true;
    }
    else if (token == "upload_dir")
    {
        if (location.upl_dir_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);
        
        location.set_upload_dir(extract_upload_dir(tokens_queue, file_name));

        location.upl_dir_is_set = true;
    }
    else if (token == "index")
    {
        if (location.index_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);
        
        location.set_upload_dir(extract_index(tokens_queue, file_name));

        location.index_is_set = true;
    }
    else if (token == "allowed_methods")
    {
        if (location.methods_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);

        location.set_allowed_methods(extract_allowed_methods(tokens_queue, file_name));

        location.methods_is_set = true;
    }
    else if (token == "return")
    {
        if (location.redirect_is_set)
            throw_config_parse_exception("duplication", token, file_name, tokens_queue.front().line_num);
        
        location.set_redirection(extract_redirection_info(tokens_queue, file_name));

        location.redirect_is_set = true;
    }
    
}

void    store_config(HttpContext& http_config, std::queue<token_info>& tokens_queue, std::string file_name, std::string context)
{
    bool    is_inside_serv = false;
    
    if (tokens_queue.front().token != "{")
    {
        throw_config_parse_exception("no_openning", context, file_name, tokens_queue.front().line_num);
    }

    tokens_queue.pop();

    while (!tokens_queue.empty() && tokens_queue.front().token != "}")
    {
        /* Here i will be extracting directives and store them one by one */
        if (context == "http")
        {
            is_inside_serv = false;

            store_http_directives(http_config, tokens_queue, file_name);
        }
        else if (context == "server")
        {
            if (!is_inside_serv)
                http_config.set_new_server();
            is_inside_serv = true;
            store_serv_directives(http_config, tokens_queue, file_name);
        }
        else if (context == "location")
        {
            store_location_directives(http_config.get_latest_server().get_latest_location(), tokens_queue, file_name);
        }
    }

    if (tokens_queue.empty())
    {
        throw_config_parse_exception("unclosed_ctx", "", file_name, tokens_queue.front().line_num);
    }

    tokens_queue.pop();

    if (context == "http" && !tokens_queue.empty())
    {
        throw_config_parse_exception("Unexpected", tokens_queue.front().token, file_name, tokens_queue.front().line_num);
    }
    
}