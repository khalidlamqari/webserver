/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 15:07:46 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/04 01:57:15 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "server_setup/Server.hpp"
#include "config_file_parsing/HttpConfigParser.hpp"


        /* ----------------------------------------------------------------------------------------------------------------- */

/*  Config parsing functions : @ /config_file_parsing/ */

    // Error throwing :

void                                    throw_config_parse_exception(std::string type, std::string token, std::string file_name, unsigned int line_num);
void                                    throw_wrong_value_exception(std::string directive, std::string wrong_value, std::string file_name, unsigned int line_num);

        /* ----------------------------------------------------------------------------------------------------------------- */


#endif