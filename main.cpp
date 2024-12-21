/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 15:05:35 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/21 11:41:52 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

// void f()
// {
// 	system("leaks webserv");
// }

int main(int argc, char *argv[])
{
	// atexit(f);
	std::string file_name;

	/*  Better create a function called usageERR to show this message! */
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
		HttpContext                         http_config;
		Server                              server;

		http_config = ConfigParser::getConfig(file_name);
		server.setup(http_config);
		server.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	catch(const char* err_msg)
	{
		std::cerr << err_msg << '\n';
		return (1);
	}
}