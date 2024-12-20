
#include "HttpConfigParser.hpp"


ConfigTokenizer::ConfigTokenizer()
{
	line_num = 1;
	pair_quote_found = false;
}

ConfigTokenizer::~ConfigTokenizer()
{
	if (file.is_open())
		file.close();
}

std::queue<token_info>   ConfigTokenizer::tokenize(const std::string & file_name)
{
    ConfigTokenizer tokenizer;

    tokenizer.open_config_file(file_name);
	return tokenizer.start_tokenization();
}

void	ConfigTokenizer::open_config_file(std::string file_name)
{
	file.open(file_name);

	if (!file.is_open())
		throw std::runtime_error("Failed to open the configuration file!");

	this->file_name = file_name;
}

std::queue<token_info>	ConfigTokenizer::start_tokenization()
{
	while (std::getline(file, line))
    {
        streamed_line.clear();
        streamed_line.str(line);

        remove_leading_spaces(); 		// Ensures the line is stripped of leading whitespace, making it easier to detect and handle lines that are empty or contain only whitespace.
		process_line_tokens();

        line_num++;
    }
	return this->tokens_queue;
}

void	ConfigTokenizer::process_line_tokens()
{
	while ((streamed_line.tellg() != -1) && ((size_t) streamed_line.tellg() != streamed_line.str().length()))
	{
		remove_leading_spaces();

		quote = leading_quote_or_wspace(streamed_line.str().substr(streamed_line.tellg()));
		if (quote != '-')
			extract_quoted_token();
		else
			streamed_line >> token;

			append_token_to_queue();

		token = "";
	}
}

void	ConfigTokenizer::extract_quoted_token()
{
	while (true)
	{
		token += get_quoted_string();
		token += capture_chars_after_quote();
		if (!pair_quote_found)
		{
			if ((streamed_line.tellg() == -1) || ((size_t) streamed_line.tellg() == streamed_line.str().length()))
			{
				token += '\n';
				if (!std::getline(file, line))
				{
					token_info	t = {"", line_num, false};
					ConfigException::throwParsingError(END_OF_FILE, t);
				}
				streamed_line.clear();
				streamed_line.str(line);
				line_num++;
			}
		}
		else
			break ;
	}

	// Reseting the pair_quote_found flag.
	pair_quote_found = false;
}

void	ConfigTokenizer::append_token_to_queue()
{
	std::string chunk;

    while (token.length())
    {
        if (token[0] == '#')
		{
			streamed_line.str("");
			streamed_line.clear();
            break;
		}

        chunk = get_next_chunk();

        token.erase(0, chunk.length());

        tokens_queue.push(normalize_token(chunk));
    }
}

std::string	ConfigTokenizer::get_quoted_string()
{
	std::string quoted;
    static int  counter;
	bool		escaped = false;

    for (size_t i = streamed_line.tellg(); i < streamed_line.str().length(); i++)
    {
        if (!escaped && (streamed_line.str()[i] == quote))
            counter++;

        quoted += streamed_line.str()[i];

        if (counter == 2)
        {
			pair_quote_found = true;
            counter = 0;
            break ;
        }

		if (line[i] == '\\' && !escaped)
			escaped = true;
		else
			escaped = false;
    }

    char buff[quoted.length() + 1];

    streamed_line.read(buff, quoted.length());
	return quoted;
}

std::string	ConfigTokenizer::capture_chars_after_quote()
{
	char 		c;
	std::string	post_quote_data;
	bool		escaped = false;

	// append untill the next space | quote
	while ((streamed_line.tellg() != -1) && ((size_t) streamed_line.tellg() != streamed_line.str().length()))
	{
		if (std::isspace(streamed_line.peek()))
		{
			break ;
		}
		streamed_line.get(c);
		post_quote_data += c;

		if (!escaped && (c == '\'' || c == '"'))
		{
			quote = c;
			pair_quote_found = false;
			break ;
		}

		if (c == '\\' && !escaped)
			escaped = true;
		else
			escaped = false;
	}
	return post_quote_data;
}

std::string	ConfigTokenizer::get_next_chunk()
{
	bool	escaped = false;
	bool	quoted = false;
	char	_quote;

	for (size_t i = 0; i < token.length(); i++)
    {
		if (!escaped && (token[i] == '\'' || token[i] == '"'))
		{
			if (quoted && token[i] == _quote)
				quoted = false;
			else if (!quoted)
			{
				_quote = token[i];
				quoted = true;
			}
		}
		else if (!quoted && !escaped && (token[i] == ';' || token[i] == '{' || token[i] == '}' || token[i] == '#'))
		{
			if (!i)
                return token.substr(0, 1);
            else
                return token.substr(0, i);
		}

		if (token[i] == '\\' && !escaped)
			escaped = true;
		else
			escaped = false;
	}
	return token;
}
 
token_info	ConfigTokenizer::normalize_token(std::string & _token)
{
	token_info	info;
	bool		is_possible_sep = true;
	bool		escaped = false;
	bool		quoted = false;
	bool		removed_one = false;
	char		_quote;

    info.line_num = line_num;

	for (size_t i = 0; i < _token.length(); i++)
	{
		if (removed_one && i--)
			removed_one = false;

		if (!escaped && (_token[i] == '\'' || _token[i] == '"'))
		{
			if (quoted && _token[i] == _quote)
				quoted = false;
			else if (!quoted)
			{
				_quote = _token[i];
				quoted = true;
			}

			if (_token[i] == _quote)
			{
				_token.erase(i, 1);
				removed_one = true;
			}
		}

		if (_token[i] == '\\' && !escaped)
		{
			is_possible_sep = false;
			_token.erase(i, 1);
			removed_one = true;
			escaped = true;
		}
		else
			escaped = false;
	}

	info.token = _token;

	if (is_possible_sep && (_token == ";" || _token == "{" || _token == "}"))
		info.is_sep = true;
	else
		info.is_sep = false;
	
	return info;
}

/* --------------------- Utils --------------------- */

void	ConfigTokenizer::remove_leading_spaces()
{
    char ch;

	while ((streamed_line.tellg() != -1)
			&& ((size_t) streamed_line.tellg() != streamed_line.str().length())
			&& (streamed_line.peek() == 32 || (streamed_line.peek() >= 9 && streamed_line.peek() <= 13)))
    {
        streamed_line.get(ch);
    }
}

char	ConfigTokenizer::leading_quote_or_wspace(const std::string & line)
{
	bool	escaped = false;

	for (size_t i = 0; i < line.length(); i++)
	{
		if (!escaped && (line[i] == '"' || line[i] == '\''))
			return line[i];
		else if (isspace(line[i]))
			return '-';

		if (line[i] == '\\' && !escaped)
			escaped = true;
		else
			escaped = false;
	}
	return '-';
}