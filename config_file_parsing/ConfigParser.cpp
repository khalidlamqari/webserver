

#include "HttpConfigParser.hpp"


ConfigParser::ConfigParser(std::queue<token_info> & tokens) : tokens_queue(tokens), extractor(tokens)
{

}


void	ConfigParser::validate_config()
{
	const std::vector<ServerContext> servers = http_config.get_servers();

	if (servers.size() == 0)
	{
		throw std::invalid_argument("Configuration Error: Please specify at least one server within the Http context.");
	}

	std::vector<ServerContext>::const_iterator it = http_config.get_servers().begin();
	std::vector<ServerContext>::const_iterator end = http_config.get_servers().end();

	for (; it < end; it++)
	{
		if (it->get_host() == "")
			throw std::invalid_argument("Configuration Error: Hostname required for server definition but not provided."); 

		if (it->get_root_directory() == "")
			throw std::invalid_argument("Configuration Error: Root directory required for server definition but not provided."); 

		if (it->get_ports().size() == 0)
			throw std::invalid_argument("Configuration Error: Please specify a port for all servers to listen on."); 
	}

}

HttpContext	ConfigParser::getConfig(const std::string & file_name)
{
	// Tokenizing the config file :
	std::queue<token_info>  tokens = ConfigTokenizer::tokenize(file_name);

	// Setting the file_name attribute for the ConfigException class.
	ConfigException::file_name = file_name;

	validate_tokens_queue(tokens);

	ConfigParser	parser(tokens);

	// Removing the "http" token from the top of the queue to start processing directives.
	parser.tokens_queue.pop();

	parser.storeConfig("http");

	parser.validate_config();

	return parser.http_config;
}

void    ConfigParser::validate_tokens_queue(const std::queue<token_info> & tokens_queue)
{
	if (tokens_queue.empty())
		ConfigException::throwParsingError(EMPTY, tokens_queue.front());

	// Making sure the first token value is "http"
	if (tokens_queue.front().token != "http")
	{
		if (tokens_queue.front().is_sep)
			ConfigException::throwParsingError(UNEXPECTED, tokens_queue.front());
		else if (is_a_valid_dir(tokens_queue.front().token))
			ConfigException::throwParsingError(NOT_ALLOWED, tokens_queue.front());
		else
			ConfigException::throwParsingError(UNKNOWN, tokens_queue.front());
	}
}

void	ConfigParser::storeConfig(const std::string & context)
{
	if (tokens_queue.front().token != "{")
		ConfigException::throwParsingError(NO_OPENING, tokens_queue.front());

	tokens_queue.pop();

	while (!tokens_queue.empty())
	{
		if (tokens_queue.front().is_sep && tokens_queue.front().token == "}")
			break ;

		if (context == "http")
			storeHttpDirs();
		else if (context == "server")
			storeServDirs();
		else if (context == "location")
			storelocationDirs();
	}

	validate_context_closure(context);
}

void	ConfigParser::validate_context_closure(const std::string & context)
{
	if (tokens_queue.empty())
		ConfigException::throwParsingError(UNCLOSED_CTX, tokens_queue.front());

	tokens_queue.pop();

	if (context == "http" && !tokens_queue.empty())
		ConfigException::throwParsingError(UNEXPECTED, tokens_queue.front());
}

void	ConfigParser::find_bad_token_type(token_info & token)
{
	if (token.is_sep)
		ConfigException::throwParsingError(UNEXPECTED, token);
	else if (is_a_valid_dir(token.token))
		ConfigException::throwParsingError(NOT_ALLOWED, tokens_queue.front());
	else
		ConfigException::throwParsingError(UNKNOWN, tokens_queue.front());
}

void	ConfigParser::storeHttpDirs()
{
	token_info	token = tokens_queue.front();

	if (!is_http_ctx_dir(token.token))
		find_bad_token_type(token);

	if (token.token == SERVER_DIR)
	{
		setup_new_server();
		return storeConfig("server");
	}

	if (is_duplicated_http_dir(token.token))
		ConfigException::throwParsingError(DUPLICATION, token);

	if (token.token == AUTO_INDX_DIR)
	{
		std::string	value = extractor.extract_single_string_value(&ConfigValueExtractor::validate_auto_indx_value);
		http_config.set_auto_index(value);
		http_config.auto_ind_is_set = true;
	}
	else if (token.token == CGI_EXCT_DIR)
	{
		std::string	value = extractor.extract_single_string_value(&ConfigValueExtractor::validate_cgi_ext_value);
		http_config.set_cgi_extension(value);
		http_config.cgi_ext_is_set = true;

	}
	else if (token.token == MAX_BODY_DIR)
	{
		size_t	value = extractor.extract_max_body_size();
		http_config.set_max_body_size(value);
		http_config.max_body_is_set = true;
	}
	else if (token.token == ERR_PAGE_DIR)
	{
		t_error_page	value = extractor.extract_error_page_info();
		http_config.set_error_page(value);
	}
}

void	ConfigParser::storeServDirs()
{
	token_info		token = tokens_queue.front();

	if (!is_server_ctx_dir(token.token))
		find_bad_token_type(token);

	if (token.token == LOCATION_DIR)
	{
		setup_new_location();
		return storeConfig("location");
	}

	ServerContext&	latest_server = http_config.get_latest_server();

	if (is_duplicated_serv_dir(token.token, latest_server))
		ConfigException::throwParsingError(DUPLICATION, token);

	if (token.token == AUTO_INDX_DIR)
	{
		std::string	value = extractor.extract_single_string_value(&ConfigValueExtractor::validate_auto_indx_value);
		latest_server.set_auto_index(value);
		latest_server.auto_ind_is_set = true;
	}
	else if (token.token == ERR_PAGE_DIR)
	{
		t_error_page	value = extractor.extract_error_page_info();
		latest_server.set_error_page(value);
	}
	else if (token.token == CGI_EXCT_DIR)
	{
		std::string	value = extractor.extract_single_string_value(&ConfigValueExtractor::validate_cgi_ext_value);
		latest_server.set_cgi_extension(value);
		latest_server.cgi_ext_is_set = true;
	}
	else if (token.token == LISTEN_DIR)
	{
		std::vector<unsigned short> value = extractor.extract_port_nums();
		latest_server.set_ports(value);
		latest_server.port_is_set = true;
	}
	else if (token.token == ROOT_DIR)
	{
		std::string	value = extractor.extract_single_string_value(NULL);
		latest_server.set_root_directory(value);
		latest_server.root_is_set = true;
	}
	else if (token.token == UPLOAD_DIR)
	{
		std::string	value = extractor.extract_single_string_value(NULL);
		latest_server.set_upload_dir(value);
		latest_server.upl_dir_is_set = true;
	}
	else if (token.token == INDEX_DIR)
	{
		std::string	value = extractor.extract_single_string_value(NULL);
		latest_server.set_index(value);
		latest_server.index_is_set = true;
	}
	else if (token.token == SERVER_NAMES_DIR)
	{
		std::vector<std::string>	value = extractor.extract_multi_string_value(NULL);
		latest_server.set_server_names(value);
		latest_server.srv_names_is_set = true;
	}
	else if (token.token == ALLOWED_METHODS_DIR)
	{
		std::vector<std::string>	value = extractor.extract_multi_string_value(&ConfigValueExtractor::validate_method);
		latest_server.set_allowed_methods(value);
		latest_server.methods_is_set = true;
	}
	else if (token.token == HOST_DIR)
	{
		std::string	value = extractor.extract_single_string_value(NULL);
		latest_server.set_host(value);
		latest_server.host_is_set = true;
	}
}

void	ConfigParser::storelocationDirs()
{
	token_info		token = tokens_queue.front();

	if (!is_location_ctx_dir(token.token))
		find_bad_token_type(token);

	LocationContext&	latest_location = http_config.get_latest_server().get_latest_location();

	if (is_duplicate_location_dir(token.token, latest_location))
		ConfigException::throwParsingError(DUPLICATION, token);

	if (token.token == AUTO_INDX_DIR)
	{
		std::string	value = extractor.extract_single_string_value(&ConfigValueExtractor::validate_auto_indx_value);
		latest_location.set_auto_index(value);
		latest_location.auto_ind_is_set = true;
	}
	else if (token.token == CGI_EXCT_DIR)
	{
		std::string	value = extractor.extract_single_string_value(&ConfigValueExtractor::validate_cgi_ext_value);
		latest_location.set_cgi_extension(value);
		latest_location.cgi_ext_is_set = true;
	}
	else if (token.token == ERR_PAGE_DIR)
	{
		t_error_page	value = extractor.extract_error_page_info();
		latest_location.set_error_page(value);
	}
	else if (token.token == ROOT_DIR)
	{
		std::string	value = extractor.extract_single_string_value(NULL);
		latest_location.set_root_directory(value);
		latest_location.root_is_set = true;
	}
	else if (token.token == UPLOAD_DIR)
	{
		std::string	value = extractor.extract_single_string_value(NULL);
		latest_location.set_upload_dir(value);
		latest_location.upl_dir_is_set = true;
	}
	else if (token.token == INDEX_DIR)
	{
		std::string	value = extractor.extract_single_string_value(NULL);
		latest_location.set_index(value);
		latest_location.index_is_set = true;
	}
	else if (token.token == ALLOWED_METHODS_DIR)
	{
		std::vector<std::string>	value = extractor.extract_multi_string_value(&ConfigValueExtractor::validate_method);
		latest_location.set_allowed_methods(value);
		latest_location.methods_is_set = true;
	}
	else if (token.token == REDIRECTION_DIR)
	{
		t_redirection_info	value = extractor.extract_redirection_info();
		latest_location.set_redirection(value);
		latest_location.redirect_is_set = true;
	}
}

bool	ConfigParser::is_duplicated_http_dir(const std::string & directive)
{
	return (((directive == AUTO_INDX_DIR) && http_config.auto_ind_is_set)
			|| ((directive == CGI_EXCT_DIR) && http_config.cgi_ext_is_set)
			|| ((directive == MAX_BODY_DIR) && http_config.max_body_is_set));
}

bool	ConfigParser::is_duplicated_serv_dir(const std::string & directive, const ServerContext & serv)
{
	return (((directive == AUTO_INDX_DIR) && serv.auto_ind_is_set)
			|| ((directive == CGI_EXCT_DIR) && serv.cgi_ext_is_set)
			|| ((directive == HOST_DIR) && serv.host_is_set)
			|| ((directive == ROOT_DIR) && serv.root_is_set)
			|| ((directive == LISTEN_DIR) && serv.port_is_set)
			|| ((directive == INDEX_DIR) && serv.index_is_set)
			|| ((directive == UPLOAD_DIR) && serv.upl_dir_is_set)
			|| ((directive == ALLOWED_METHODS_DIR) && serv.methods_is_set)
			|| ((directive == SERVER_NAMES_DIR) && serv.srv_names_is_set));
}

bool	ConfigParser::is_duplicate_location_dir(const std::string & directive, const LocationContext & location)
{
	return (((directive == AUTO_INDX_DIR) && location.auto_ind_is_set)
			|| ((directive == CGI_EXCT_DIR) && location.cgi_ext_is_set)
			|| ((directive == ROOT_DIR) && location.root_is_set)
			|| ((directive == INDEX_DIR) && location.index_is_set)
			|| ((directive == UPLOAD_DIR) && location.upl_dir_is_set)
			|| ((directive == ALLOWED_METHODS_DIR) && location.methods_is_set)
			|| ((directive == REDIRECTION_DIR) && location.redirect_is_set));
}

void	ConfigParser::setup_new_server()
{
	http_config.set_new_server();
	tokens_queue.pop(); 		// removing the "server" directive token.
}

void	ConfigParser::setup_new_location()
{
	std::string	location = extractor.extract_location();

	http_config.get_latest_server().set_new_location(location);
}