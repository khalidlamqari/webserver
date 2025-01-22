
#include "webserv.hpp"
#include <signal.h>

static bool	validate_file_name(const std::string & file_name)
{
	if ((file_name.length() <= 5) || (file_name.find(".conf" ,file_name.length() - 5) == std::string::npos))
	{
		std::cerr << "Wrong file extension!" << std::endl;
		std::cerr << "Usage : ./webserv file.config" << std::endl;
		return 0;
	}
	return 1;
}

static int	setupAndStartWebServer(const std::string & file_name)
{
	try
	{
		HttpContext	http_config;
		Server		server;

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
	catch(const std::string & err_msg)
	{
		std::cerr << err_msg << '\n';
		return (1);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	signal(SIGPIPE, SIG_IGN);

	std::string file_name;

	if (argc == 2)
	{
		file_name = argv[1];
		if (!validate_file_name(file_name))
			return 1;
	}
	else if (argc == 1)
	{
		file_name = "Configs/default.conf";
	}
	else
	{
		std::cerr << "Usage : ./webserv file.config" << std::endl;
		return (1);
	}

	return setupAndStartWebServer(file_name);
}