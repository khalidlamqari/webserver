

#include "HttpConfigParser.hpp"

ConfigValueExtractor::ConfigValueExtractor(std::queue<token_info> & tokens) : tokens_queue(tokens)
{

}

token_info &	ConfigValueExtractor::front_token(const token_info & directive)
{
	if (tokens_queue.empty())
		ConfigException::throwParsingError(UNTERMINATED, directive);
	return tokens_queue.front();
}

std::string	ConfigValueExtractor::extract_single_string_value(void (ConfigValueExtractor::*validator)(const token_info &))
{
	std::string	value;
	token_info	directive;
	token_info	token;

	directive = tokens_queue.front();

    tokens_queue.pop();
    token = front_token(directive);

	if (!tokens_queue.empty() && token.is_sep)
		ConfigException::throwParsingError(UNEXPECTED, token);
	else
		value = token.token;

	if (validator)
		(this->*validator)(token);

	tokens_queue.pop();
	token = front_token(directive);

	validate_directive_ending(token, directive);

	tokens_queue.pop();
	return value;
}

std::vector<std::string>	ConfigValueExtractor::extract_multi_string_value(void (ConfigValueExtractor::*validator)(const token_info &))
{
	std::vector<std::string>	value;
	token_info					directive;
	token_info					token;

	directive = tokens_queue.front();

    tokens_queue.pop();
    token = front_token(directive);

	if (!tokens_queue.empty() && token.is_sep)
		ConfigException::throwParsingError(UNEXPECTED, token);

	while (!tokens_queue.empty() && !token.is_sep)
	{
		if (validator)
			(this->*validator)(token);

		value.push_back(token.token);
		tokens_queue.pop();
		token = front_token(directive);
	}

	validate_directive_ending(token, directive);

	tokens_queue.pop();
	return value;
}

std::vector<unsigned short>	ConfigValueExtractor::extract_port_nums()
{
	std::vector<unsigned short>	value;
	token_info					directive;
	token_info					token;

	directive = tokens_queue.front();

    tokens_queue.pop();
    token = front_token(directive);

	if (!tokens_queue.empty() && token.is_sep)
		ConfigException::throwParsingError(UNEXPECTED, token);

	while (!tokens_queue.empty() && !token.is_sep)
	{
		validate_port_number(token);
		value.push_back(my_stoi(token.token));
		tokens_queue.pop();
		token = front_token(directive);
	}

	validate_directive_ending(token, directive);

	tokens_queue.pop();
	return value;
}

size_t    ConfigValueExtractor::extract_max_body_size()
{
	token_info	token;
	token_info	directive;
	size_t		size = 0;

	directive = tokens_queue.front();

    tokens_queue.pop();
    token = front_token(directive);

    if (!tokens_queue.empty() && token.is_sep)
		ConfigException::throwParsingError(UNEXPECTED, token);

	validate_max_body_size(token);

	size = my_stoul(token.token);

    tokens_queue.pop();
    token = front_token(directive);

    validate_directive_ending(token, directive);

	tokens_queue.pop();
    return size;
}

t_error_page	ConfigValueExtractor::extract_error_page_info()
{
	t_error_page	info;
	token_info		token;
	token_info		directive;

	directive = tokens_queue.front();

    tokens_queue.pop();
    token = front_token(directive);

	if (!tokens_queue.empty() && token.is_sep)
		ConfigException::throwParsingError(UNEXPECTED, token);
	
	validate_http_code_value(token);

	info.err_code = my_stoi(token.token);

	tokens_queue.pop();
    token = front_token(directive);

	if (!tokens_queue.empty() && token.is_sep && token.token == ";")
		ConfigException::throwParsingError(WRONG_ARGS_NUM, directive);
	if (!tokens_queue.empty() && token.is_sep)
		ConfigException::throwParsingError(UNEXPECTED, token);

	info.path = token.token;

	tokens_queue.pop();
    token = front_token(directive);

    validate_directive_ending(token, directive);

	tokens_queue.pop();
	return info;
}

std::pair<extension, execPath>	ConfigValueExtractor::extract_cgi_info()
{
	std::pair<extension, execPath>	info;
	token_info						token;
	token_info						directive;

	directive = tokens_queue.front();

	tokens_queue.pop();
    token = front_token(directive);

	if (!tokens_queue.empty() && token.is_sep)
		ConfigException::throwParsingError(UNEXPECTED, token);

	info.first = token.token;

	tokens_queue.pop();
    token = front_token(directive);

	if (!tokens_queue.empty() && token.is_sep && token.token == ";")
		ConfigException::throwParsingError(WRONG_ARGS_NUM, directive);
	if (!tokens_queue.empty() && token.is_sep)
		ConfigException::throwParsingError(UNEXPECTED, token);
	
	info.second = token.token;

	tokens_queue.pop();
    token = front_token(directive);

    validate_directive_ending(token, directive);

	tokens_queue.pop();
	return info;
}

std::string	ConfigValueExtractor::extract_location()
{
	std::string	value;
	token_info	token;
	token_info	directive;

	directive = tokens_queue.front();
	tokens_queue.pop();

	token = front_token(directive);

	if (!tokens_queue.empty() && token.is_sep)
		ConfigException::throwParsingError(UNEXPECTED, token);
	
	tokens_queue.pop();
	return token.token;
}

t_redirection_info			ConfigValueExtractor::extract_redirection_info()
{
	t_redirection_info	info;
	token_info			token;
	token_info			directive;

	directive = tokens_queue.front();

	tokens_queue.pop();
	token = front_token(directive);

	if (!tokens_queue.empty() && token.is_sep)
		ConfigException::throwParsingError(UNEXPECTED, token);

	validate_redirection_code(token);

	info.status_code = my_stoi(token.token);

	tokens_queue.pop();
    token = front_token(directive);

	if (!tokens_queue.empty() && token.is_sep && token.token != ";")
		ConfigException::throwParsingError(UNTERMINATED, directive);
	else if(!tokens_queue.empty() && !token.is_sep)
	{
		info.target = token.token;
		tokens_queue.pop();
	}

	token = front_token(directive);
	validate_directive_ending(token, directive);

	tokens_queue.pop();
	return info;
}

time_t	ConfigValueExtractor::extract_time_value()
{
	token_info	token;
	token_info	directive;
	time_t		time = 0;

	directive = tokens_queue.front();

    tokens_queue.pop();
    token = front_token(directive);

    if (!tokens_queue.empty() && token.is_sep)
		ConfigException::throwParsingError(UNEXPECTED, token);

	validate_time_value(token);

	time = my_stoul(token.token);

    tokens_queue.pop();
    token = front_token(directive);

    validate_directive_ending(token, directive);

	tokens_queue.pop();
    return time;
}

void	ConfigValueExtractor::validate_directive_ending(const token_info & token, const token_info & directive)
{
	if (!tokens_queue.empty() && token.is_sep && token.token == ";")
		return ;

	if (tokens_queue.empty() || token.is_sep)
		ConfigException::throwParsingError(UNTERMINATED, directive);
	else
		ConfigException::throwParsingError(WRONG_ARGS_NUM, directive);
}

void	ConfigValueExtractor::validate_port_number(const token_info & token)
{
	if (!is_all_digits(token.token)
		|| (token.token.length() != 4 && token.token.length() != 5)
		|| (token.token.length() == 4 && token.token < "1024" )
		|| (token.token.length() == 5 && token.token > "49151"))
	{	
		ConfigException::throwWrongValueError(LISTEN_DIR, token);
	}
}

void	ConfigValueExtractor::validate_max_body_size(const token_info & token)
{
	if (!is_all_digits(token.token)
		|| (token.token.length() > 11)
		|| ((token.token.length() == 11) && (token.token > "12500000000")))
	{
		ConfigException::throwWrongValueError(MAX_BODY_DIR, token);
	}
}

void	ConfigValueExtractor::validate_auto_indx_value(const token_info & token)
{
	if ((token.token != "on") && (token.token != "off"))
		ConfigException::throwWrongValueError(AUTO_INDX_DIR, token);
}

void	ConfigValueExtractor::validate_http_code_value(const token_info & token)
{
	if (!is_all_digits(token.token)
		|| (token.token.length() != 3)
		|| (token.token < "300")
		|| (token.token > "599"))
	{
		ConfigException::throwWrongValueError(ERR_PAGE_DIR, token);
	}
}

void	ConfigValueExtractor::validate_redirection_code(const token_info & token)
{
	if (!is_all_digits(token.token)
		|| token.token.length() > 3)
	{
		ConfigException::throwWrongValueError(REDIRECTION_DIR, token);
	}
}

void	ConfigValueExtractor::validate_method(const token_info & token)
{
	if ((token.token != "GET")
		&& (token.token != "POST")
		&& (token.token != "DELETE")
		&& (token.token != "HEAD"))
	{
		ConfigException::throwWrongValueError(ALLOWED_METHODS_DIR, token);
	}
}

void	ConfigValueExtractor::validate_time_value(const token_info & token)
{
	if (!is_all_digits(token.token)
		|| (token.token.length() > 19)
		|| ((token.token.length() == 19) && (token.token > "9223372036854775807"))
		|| ((token.token.length() == 1) && (token.token < "3")))
	{
		ConfigException::throwWrongValueError(CGI_RD_TMOUT_DIR, token);
	}
}