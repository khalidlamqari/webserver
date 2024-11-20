/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_name_checker.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 17:12:39 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/18 01:11:35 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

bool    is_http_ctx_dir(std::string directive)
{
    return  (directive == "error_page" || directive == "server" || directive == "cgi_extention"
            || directive == "autoindex" || directive == "client_max_body_size");
}

bool    is_server_ctx_dir(std::string directive)
{
    return  (directive == "error_page" || directive == "location" || directive == "listen"
            || directive == "root" || directive == "upload_dir" || directive == "index"
            || directive == "server_names" || directive == "cgi_extention"
            || directive == "autoindex" || directive == "allowed_methods"
            || directive == "host");
}

bool    is_location_ctx_dir(std::string directive)
{
    return  (directive == "error_page" || directive == "root" || directive == "upload_dir" // remove upload dir
            || directive == "index" || directive == "cgi_extention" || directive == "autoindex"
            || directive == "return" || directive == "allowed_methods");
}
