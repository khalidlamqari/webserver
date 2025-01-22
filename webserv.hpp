
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