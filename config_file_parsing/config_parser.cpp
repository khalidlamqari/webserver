/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymafaman <ymafaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 15:04:07 by ymafaman          #+#    #+#             */
/*   Updated: 2024/11/09 16:14:38 by ymafaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

/* Removing the escape characters from the token before storing it */

static token_info normalize_token(std::string token, unsigned int line_num)
{
    token_info  info;
    bool        removed_one;
    bool        escaped;
    char        quote;

    if (token == ";" || token == "{" || token == "}")
    {
        info.token = token;
        info.line_num = line_num;
        info.is_sep = true;
        return info;
    }

    removed_one = false;
    escaped = false;
    
    /* if i remove quotes first then do the following step i will not know if the character im trying to escape was withing quotes or not. */
    for (size_t i = 0; i < token.length(); i++)
    {
        if (removed_one)
            i--;
         
        if ((token[i] == '\\') && !removed_one && (i != token.length() - 1) && ((token[i + 1] == '\\')
            || (token[i + 1] == ';') || (token[i + 1] == '{') || (token[i + 1] == '}')
            || (token[i + 1] == '#') || (token[i + 1] == '\'') || (token[i + 1] == '"')))
        {
            token.erase(i, 1);
            escaped = true;
            removed_one = true;
        }
        else if ((token[i] == '\'' || token[i] == '"') && !escaped)
        {
            quote = token[i];
            token.erase(i, 1);
            removed_one = true;
            
            int escape_char_count = 0;

            while (i < token.length())
            {
                if ((token[i] == quote && i == 0) || (token[i] == quote && token[i - 1] != '\\'))
                {
                    token.erase(i, 1);
                    break ;
                }
                else if (token[i] == '\\' && escape_char_count == 1)
                {
                    token.erase(i - 1, 1);
                    i--;
                    escape_char_count = 0;

                }        
                else if (token[i] == '\\')
                {
                    escape_char_count++;
                }
                else if ((token[i] == quote) && (token[i - 1] == '\\') && (escape_char_count % 2))
                {
                    token.erase(i - 1, 1);
                    i--;
                }        

                if (token[i] != '\\')
                {
                    escape_char_count = 0;
                }
                i++;
            }
        }
        else
        {
            escaped = false;   
            removed_one = false;
        }
    }

    info.token = token;
    info.line_num = line_num;
    info.is_sep = false;

    return info;
}

static std::string get_next_chunk(std::string token)
{
    unsigned int    escape_char_counter;
    char            quote;

    escape_char_counter = 0;

    for (size_t i = 0; i < token.length(); i++)
    {
        if ((token[i] == '"' || token[i] == '\'') && !(escape_char_counter % 2))
        {
            quote = token[i];
            i++;
            while (i < token.length())
            {
                if ((token[i] == quote) && !(escape_char_counter % 2))
                    break ;
                i++;
            }
            i++;
        }

        if (token[i] == '\\')
            escape_char_counter++;

        if ((token[i] == ';' || token[i] == '{' || token[i] == '}') && !(escape_char_counter % 2))
        {
            if (!i)
                return token.substr(0, 1);
            else
                return token.substr(0, i);
        }

        if (token[i] != '\\')
            escape_char_counter = 0;
    }
    return token;
}

/* this function's job is to only store tokens and skip comments */

static bool    has_unmatched_quote(std::string token)
{
    unsigned int    escape_char_counter;
    bool            unmatched_quote_found;
    char            quote;

    escape_char_counter = 0;

    for (size_t i = 0; i < token.length(); i++)
    {
        if (token[i] == '\\')
        {
            escape_char_counter++;
        }
        else if (!unmatched_quote_found && (token[i] == '\'' || token[i] == '"') && !(escape_char_counter % 2))
        {
            unmatched_quote_found = true;
            quote = token[i];
        }
        else if (unmatched_quote_found && (token[i] == quote) && !(escape_char_counter % 2))
        {
            unmatched_quote_found = false;
        }

        if (token[i] != '\\')
            escape_char_counter = 0;
    }
    return unmatched_quote_found;
}

static void    append_token_to_queue(std::string token, std::queue<token_info>& tokens_queue, unsigned int line_num)
{
    std::string chunk;
    size_t      chunk_pos;

    while (token.length())
    {
        if (token[0] == '#')
            break;

        chunk = get_next_chunk(token);

        tokens_queue.push(normalize_token(chunk, line_num));

        chunk_pos = token.find(chunk);
        token.erase(chunk_pos, chunk.length());
    }
}

static char    quote_first(std::string str)
{    
    for (size_t i = 0; i < str.length(); i++)
    {
        if ((i == 0) && (str[i] == '"' || str[i] == '\''))
        {
            return str[i];
        }
        else if (i != 0)
        {
            if ((str[i - 1] != '\\') && (str[i] == '"' || str[i] == '\''))
                return str[i];
        }
        else if (is_space(str[i]))
        {
            return '-';
        }
    }

    return '-';
}

static void    remove_leading_spaces(std::stringstream& strm)
{
    while ((strm.tellg() != -1) && ((size_t) strm.tellg() != strm.str().length()) && (strm.peek() == 32 || (strm.peek() >= 9 && strm.peek() <= 13)))
    {
        char ch;
        strm.get(ch);
    }
}

static std::string get_quoted_string(std::stringstream& strm, char quote)
{
    std::string quoted;
    static int  counter;
    bool        pair_found = false;

    for (size_t i = strm.tellg(); i < strm.str().length(); i++)
    {
        if ((strm.str()[i] == quote && i == 0) || (strm.str()[i] == quote && strm.str()[i - 1] != '\\'))
        {
            counter++;
        }

        quoted += strm.str()[i];

        if (counter == 2)
        {
            pair_found = true;
            counter = 0;
            break ;
        }
    }
    
    char buff[quoted.length() + 1];

    strm.read(buff, quoted.length());

    buff[quoted.length()] = '\0'; // no need

    if (pair_found)
    {
        char c;

        // append untill the next space / quote (if it s a quote append it so that the other function calls again)
        while ((strm.tellg() != -1) && ((size_t) strm.tellg() != strm.str().length())) // find out why it fails when i dont do strm.str.length in case there is only "qeqwe"
        {
            if (is_space(strm.peek()))
            {
                break ;
            }
            strm.get(c);
            quoted += c;

            if ((c == '\'' || c == '"') && quoted[quoted.length() - 1] != '\\') // because if it s an escaped quote, the escape char will be already pushed to the string
                break ;
        }
    }

    return quoted;
}

static void    tokenize_config(std::queue<token_info>& tokens_queue, std::string file_name)
{
    std::stringstream   strm;
    unsigned int        line_num;
    std::fstream        file;
    std::string         token;
    std::string         line;
    char                quote;

    file.open(file_name);
    if (!file.is_open())
        throw std::runtime_error("Failed to open the configuration file!");

    line_num = 1;
    quote = '-';

    while (std::getline(file, line))
    {
        strm.clear();
        strm.str(line);

        remove_leading_spaces(strm);

        while ((strm.tellg() != -1) && ((size_t) strm.tellg() != strm.str().length())) // the second condition is for "qeqwe"
        {
            remove_leading_spaces(strm);

            quote = quote_first(strm.str().substr(strm.tellg()));
            if (quote != '-')
            {
                while (true)
                {
                    token += get_quoted_string(strm, quote);
                    if (has_unmatched_quote(token))
                    {
                        if ((strm.tellg() == -1) || ((size_t) strm.tellg() == strm.str().length()))
                        {
                            token += '\n';
                            if (!std::getline(file, line))
                                throw_config_parse_exception("EOF", "", file_name, line_num);
                            strm.clear();
                            strm.str(line);
                        }
                    }
                    else
                        break ;
                }
            }
            else
                strm >> token;

                append_token_to_queue(token, tokens_queue, line_num);

            token = "";
        }
        line_num++;
    }
    file.close(); // file has to be closed in all cases try to open it in main!
}

static void    validate_config(const HttpContext& http_config )
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

void    parse_config_file(std::string file_name, HttpContext& http_config)
{
    std::queue<token_info>  tokens_queue;

    tokenize_config(tokens_queue, file_name);

    if (tokens_queue.empty())
    {
        std::cerr << "empty" << std::endl;
        exit (1);
    }

    if (tokens_queue.front().token != "http")
    {
        if (tokens_queue.front().token == ";" || tokens_queue.front().token == "{" || tokens_queue.front().token == "}")
            throw_config_parse_exception("Unexpected", tokens_queue.front().token, file_name, tokens_queue.front().line_num);
        else if (is_http_ctx_dir(tokens_queue.front().token) || is_server_ctx_dir(tokens_queue.front().token) || is_location_ctx_dir(tokens_queue.front().token))
            throw_config_parse_exception("Not allowed", tokens_queue.front().token, file_name, tokens_queue.front().line_num);
        else
            throw_config_parse_exception("Unknown", tokens_queue.front().token, file_name, tokens_queue.front().line_num);
    }
    tokens_queue.pop();

    store_config(http_config, tokens_queue, file_name, "http");

    validate_config(http_config);
}