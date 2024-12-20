/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 15:05:35 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/06 23:18:27 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"


// std::string indent(int level) {
//     return std::string(level * 2, ' ');
// }

// // Adjusted Debug Functions
// void printErrorPage(const t_error_page& errorPage, int level) {
//     std::cout << indent(level) << "* Error Page -> Code: " << errorPage.err_code
//               << ", Path: " << errorPage.path << "\n";
// }

// void printRedirectionInfo(const t_redirection_info& redirectInfo, int level) {
//     std::cout << indent(level) << "* Redirection -> Status Code: " << redirectInfo.status_code
//               << ", Target: " << redirectInfo.target << "\n";
// }

// void printLocationContext(const LocationContext& location, int level) {
//     std::cout << indent(level) << "Location: " << location.location << "\n";
//     std::cout << indent(level + 1) << "- Root Directory: " << location.root_directory << "\n";
//     std::cout << indent(level + 1) << "- CGI Extension: " << location.cgi_extension << "\n";
//     std::cout << indent(level + 1) << "- Upload Directory: " << location.upload_dir << "\n";
//     std::cout << indent(level + 1) << "- Index: " << location.index << "\n";
//     std::cout << indent(level + 1) << "- Auto Index: " << (location.auto_index ? "Enabled" : "Disabled") << "\n";

//     std::cout << indent(level + 1) << "- Allowed Methods:\n";
//     for (const auto& method : location.allowed_methods) {
//         std::cout << indent(level + 2) << "* " << method << "\n";
//     }

//     printRedirectionInfo(location.redirection, level + 1);

//     for (const auto& errorPage : location.error_pages) {
//         printErrorPage(errorPage, level + 1);
//     }
// }

// void printServerContext(const ServerContext& server, int level) {
//     std::cout << indent(level) << "Server Context:\n";
//     std::cout << indent(level + 1) << "- Root Directory: " << server.root_directory << "\n";
//     std::cout << indent(level + 1) << "- CGI Extension: " << server.cgi_extension << "\n";
//     std::cout << indent(level + 1) << "- Upload Directory: " << server.upload_dir << "\n";
//     std::cout << indent(level + 1) << "- Index: " << server.index << "\n";
//     std::cout << indent(level + 1) << "- Host: " << server.host << "\n";
//     std::cout << indent(level + 1) << "- Auto Index: " << (server.auto_index ? "Enabled" : "Disabled") << "\n";

//     std::cout << indent(level + 1) << "- Ports:\n";
//     for (const auto& port : server.ports) {
//         std::cout << indent(level + 2) << "* " << port << "\n";
//     }

//     std::cout << indent(level + 1) << "- Server Names:\n";
//     for (const auto& name : server.server_names) {
//         std::cout << indent(level + 2) << "* " << name << "\n";
//     }

//     std::cout << indent(level + 1) << "- Allowed Methods:\n";
//     for (const auto& method : server.allowed_methods) {
//         std::cout << indent(level + 2) << "* " << method << "\n";
//     }

//     for (const auto& location : server.locations) {
//         printLocationContext(location, level + 1);
//     }

//     for (const auto& errorPage : server.error_pages) {
//         printErrorPage(errorPage, level + 1);
//     }
// }

// void printHttpContext(const HttpContext& httpContext) {
//     std::cout << "HTTP Context:\n";
//     std::cout << indent(1) << "- CGI Extension: " << httpContext.cgi_extension << "\n";
//     std::cout << indent(1) << "- Max Body Size: " << httpContext.max_body_size << "\n";
//     std::cout << indent(1) << "- Auto Index: " << (httpContext.auto_index ? "Enabled" : "Disabled") << "\n";

//     for (const auto& server : httpContext.servers) {
//         printServerContext(server, 1);
//     }

//     for (const auto& errorPage : httpContext.error_pages) {
//         printErrorPage(errorPage, 1);
//     }
// }

void f()
{
	system("leaks webserv");
}

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
	//     printHttpContext(http_config);
	// exit(0);
	
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