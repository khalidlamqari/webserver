
#include "HttpConfigParser.hpp"

std::string ConfigException::file_name = "";

void	ConfigException::throwParsingError(const ConfigParseError & e, const token_info & token) // TODO : change the value in token_info to value
{
	std::ostringstream	error_info;

    switch (e)
	{
        case EMPTY :
             error_info << "webserv : Configuration file is empty. Please provide a valid configuration.";
             break ;
        case UNEXPECTED :
			error_info << "webserv : unexpected \"" + token.token + "\" in " + ConfigException::file_name + ":";
			error_info << token.line_num;
			break ;
		case UNTERMINATED :
			error_info << "webserv : directive \"" + token.token + "\" is not terminated by \";\" in " + ConfigException::file_name + ":";
			error_info << token.line_num;
			break ;
		case WRONG_ARGS_NUM :
			error_info << "webserv : invalid number of arguments in \"" + token.token + "\" directive in " + ConfigException::file_name + ":";
			error_info << token.line_num;
			break ;
        case NOT_ALLOWED :
            error_info << "webserv : \"" + token.token + "\" directive is not allowed here in " + ConfigException::file_name + ":";
			error_info << token.line_num;
			break ;
		case UNKNOWN :
        	error_info << "webserv : unknown directive \"" + token.token + "\"in " + ConfigException::file_name + ":";
			error_info << token.line_num;
			break ;
        case NO_OPENING :
            error_info << "webserv : missing opening \"{\" in " + ConfigException::file_name + ":";
			error_info << token.line_num;
			break ;
        case DUPLICATION :
            error_info << "webserv : \"" + token.token + "\" directive is duplicate in " + ConfigException::file_name + ":";
			error_info << token.line_num;
            break ;
        case UNCLOSED_CTX :
            error_info << "webserv : unexpected end of file, expecting \"}\" in " + file_name + ":";
            error_info <<  token.line_num;
            break ;
        case END_OF_FILE :
            error_info << "webserv : unexpected end of file, expecting \";\" or \"}\" in " + file_name + ":";
            error_info <<  token.line_num;
    }

	throw std::invalid_argument(error_info.str()); // TODO : Better create a specific exception type
}


void    ConfigException::throwWrongValueError(const std::string & directive, const token_info & token)
{
    std::ostringstream  error_info;

    if (directive == AUTO_INDX_DIR)
    {
        error_info << "webserv : invalid value \"" + token.token + "\" in \"" + directive + "\" directive,  it must be \"on\" or \"off\" in " + file_name + ":";
        error_info <<  token.line_num;
    }
    else if (directive == ERR_PAGE_DIR)
    {
        error_info << "webserv : value \"" + token.token + "\" must be a number between 300 and 599 in " + file_name + ":";
        error_info <<  token.line_num;
    }
    else if (directive == CGI_EXCT_DIR)
    {
        error_info << "webserv : invalid value \"" + token.token + "\" in \"" + directive + "\" directive, it must be \".php\" in " + file_name + ":";
        error_info <<  token.line_num;
    }
    else if (directive == LISTEN_DIR)
    {
        error_info << "webserv : value \"" + token.token + "\" must be a number between 1024 and 49151 in " + file_name + ":";
        error_info <<  token.line_num;
    }
    else if (directive == ALLOWED_METHODS_DIR)
    {
        error_info << "webserv : invalid method \"" + token.token + "\" in " + file_name + ":";
        error_info <<  token.line_num;
    }
    else if (directive == REDIRECTION_DIR)
    {
        error_info << "webserv : invalid return code \"" + token.token + "\" in " + file_name + ":";
        error_info <<  token.line_num;
    }
    else if (directive == MAX_BODY_DIR)
    {
        error_info << "webserv : value \"" + token.token + "\" must be a number between 0 and 12500000000 in " + file_name + ":";
        error_info <<  token.line_num;
    }

    throw std::invalid_argument(error_info.str());
}