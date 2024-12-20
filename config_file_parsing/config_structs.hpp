#ifndef CONFIG_STRUCTS_HPP
#define CONFIG_STRUCTS_HPP

#include <string>

typedef struct	s_error_page
{
	unsigned short	err_code;
	std::string		path;
}	t_error_page;

typedef struct	s_redirection_info
{
	unsigned short	status_code;
	std::string		target;
}	t_redirection_info;

typedef struct token_info
{
    std::string     token;
    unsigned int    line_num;
    bool            is_sep; // This indecates if the token is a separator or not. Separators are (";" and "{" and "}")
} token_info;

// TODO : can be wrapped inside a namespace!
typedef enum	e_ConfigParseError
{
	EMPTY,
	UNEXPECTED,
	NOT_ALLOWED,
	UNKNOWN,
	UNCLOSED_CTX,
	DUPLICATION,
	UNTERMINATED,
	WRONG_ARGS_NUM,
	END_OF_FILE,
	NO_OPENING
}	ConfigParseError;



#endif