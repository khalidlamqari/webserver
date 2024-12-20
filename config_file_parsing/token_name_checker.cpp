/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_name_checker.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/11 17:12:39 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/01 03:48:59 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpConfigParser.hpp"

bool    is_http_ctx_dir(const std::string & directive)
{
    return  (directive == ERR_PAGE_DIR
            || directive == SERVER_DIR
            || directive == CGI_EXCT_DIR
            || directive == AUTO_INDX_DIR
            || directive == MAX_BODY_DIR);
}

bool    is_server_ctx_dir(const std::string & directive)
{
    return  (directive == ERR_PAGE_DIR
            || directive == LOCATION_DIR
            || directive == LISTEN_DIR
            || directive == ROOT_DIR
            || directive == UPLOAD_DIR 
            || directive == INDEX_DIR
            || directive == SERVER_NAMES_DIR
            || directive == CGI_EXCT_DIR
            || directive == AUTO_INDX_DIR
            || directive == ALLOWED_METHODS_DIR
            || directive == HOST_DIR);
}

bool    is_location_ctx_dir(const std::string & directive)
{
    return  (directive == ERR_PAGE_DIR
            || directive == ROOT_DIR
            || directive == INDEX_DIR
            || directive == CGI_EXCT_DIR
            || directive == AUTO_INDX_DIR
            || directive == REDIRECTION_DIR
            || directive == ALLOWED_METHODS_DIR);
}

bool    is_a_valid_dir(const std::string & directive)
{
    return (is_http_ctx_dir(directive)
            || is_server_ctx_dir(directive)
            || is_location_ctx_dir(directive));
}