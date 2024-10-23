/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 09:49:46 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/23 18:39:10 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"
#include <iostream>
#include "../webserv.hpp"

int main(int argc, char *argv[])
{
    std::string file_name;
    HttpContext             http_config;
    if (argc != 2)
    {
        std::cerr << "Usage : ./webserv file.config" << std::endl;
        return (1);
    }

    file_name = argv[1];
    
    if((file_name.length() <= 7) || (file_name.find(".config" ,file_name.length() - 7) == std::string::npos))
    {
        std::cerr << "Wrong file extension!" << std::endl;
        std::cerr << "Usage : ./webserv file.config" << std::endl;
        return (1);
    }

    try
    {
        config_tokenizer( file_name, http_config );
        http_config.show_info();
    }
    catch( const std::exception& e )
    {
        std::cerr << e.what() << '\n';
    }
    
    std::vector<ServerContext> server = http_config.get_servers() ;
    
    Server serv( *(server.begin()) ) ;
    serv.run() ;
    serv.closeServer() ;
    
}

