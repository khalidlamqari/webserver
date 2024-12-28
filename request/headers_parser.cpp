/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headers_parser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:05:24 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/28 10:09:54 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_parse.hpp"

static void trim_white_spaces(Request & request, std::string & header_value, const std::string & allowedWS, const std::string & forbidenWS)
{
	for (size_t i = 0; i < header_value.length() && isspace(header_value[i]); i++)
	{
		if (allowedWS.find(header_value[i]) != std::string::npos)
		{
			header_value.erase(i, 1);
			i--;
		}
		else if (forbidenWS.find(header_value[i]) != std::string::npos)
			request.markAsBad(11);
	}

	for (size_t i = header_value.length(); i > 0 && isspace(header_value[i]); i--)
	{
		if (allowedWS.find(header_value[i]))
		{
			header_value.erase(i, 1);
			i++;
		}
		else if (forbidenWS.find(header_value[i]))
			request.markAsBad(2);
	}
}

static bool contain_white_space(std::string token)
{
	for (size_t i = 0; i < token.length(); i++)
	{
		if (isspace(token[i]))
			return true;
	}
	return false;
}

static void	validate_host(Request & request, std::string & host_value)
{
	if (request.hostIsSet())
		request.markAsBad(0);

	trim_white_spaces(request, host_value, " ", "\r\n\f\t\b\v");

	// Locate the position of the colon, which separates the HOST or SERVER_NAME from the PORT in the host value (format: HOST:PORT).
	size_t		colon_pos = host_value.find(':');
	if (colon_pos != std::string::npos)
		host_value.erase(colon_pos);

	if (host_value.empty() || contain_white_space(host_value))
		request.markAsBad(3);

	request.setHeader("HOST", host_value);
	request.markHostAsSet();
}

static void normalize_header_name(std::string & name)
{
	for (size_t i = 0; i < name.length(); i++)
		name[i] = std::toupper(name[i]);
}

static void validate_te(Request & request, std::string & field_value)
{
	std::stringstream strm(field_value);
	std::string value;

	while (std::getline(strm, value, ','))
	{
		trim_white_spaces(request, value, " \t\v\f\b", "\n\r");

		if (value == "chunked")
			return request.markAsChunked();
	}
}

static void validate_connection(Request & request, std::string & field_value)
{
	std::stringstream strm(field_value);
	std::string value;

	while (std::getline(strm, value, ','))
	{
		trim_white_spaces(request, value, " \t\v\f\b", "\n\r");

		if (value == "close")
			return request.markAsPersistent(false);
		else if (value == "keep-alive")
			return request.markAsPersistent(true);
	}
}

static void validate_content_length(Request & request, std::string & field_value)
{
	for (size_t i = 0; i < field_value.length(); i++)
	{
		if (!isdigit(field_value[i]))
			request.markAsBad(99);
	}

	try
	{
		request.set_content_length(std::stoul(field_value));
	}
	catch(const std::exception& e)
	{
		request.markAsBad(100);
	}
}

static bool is_multipart(std::string & type) // TODO : check if all multipart types are important!
{
	return (    type == "multipart/mixed" 
			||  type == "multipart/related" 
			||  type == "multipart/parallel" 
			||  type == "multipart/alternative" 
			||  type == "multipart/form-data");
}

std::string    get_boundary(std::string & bundary_parameter)
{
	std::string allowed_separators  = ":;,= ";
	std::string allowed_bchars      = "'()+_-./?";
	bool        is_quoted           = false;
	std::string boundary;

	boundary = bundary_parameter.substr(9); // Extract after "boundary="

	if (boundary.front() == '"' && boundary.back() == '"')
	{
		boundary.erase(0, 1);
		boundary.erase(boundary.length() - 1, 1);
		is_quoted = true;
	}

	if (boundary.empty() || boundary.length() > 70)
		return "";

	for (size_t i = 0; i < boundary.length(); i++)
	{
		if ((!isalnum(boundary[i]) && (allowed_bchars.find(boundary[i]) == std::string::npos) && (allowed_separators.find(boundary[i]) == std::string::npos))
			|| (allowed_separators.find(boundary[i]) != std::string::npos && !is_quoted))
			return "";
	}
	return boundary;
}

static void validate_content_type(Request & request, std::string & field_value)
{
	std::stringstream   strm(field_value);
	std::string         token;
	std::string         content_type;
	std::string         boundary;
	int                 i = 0;

	while (getline(strm, token, ';'))
	{
		trim_white_spaces(request, token, " \t\v\f\b", "\n\r");

		if (i == 0 && !is_multipart(token))
			return ;
		else if ((i != 0) && (token.find("boundary=") == 0))
		{
			boundary = get_boundary(token);
			if (boundary.empty())
				return request.markAsBad(101);

			request.markAsMultipart();
			request.set_boundary(boundary);
			return ;
		}
		i++;
	}
	request.markAsBad(88); // Mark the request as invalid because the Content-Type is 'multipart', but the required 'boundary' parameter is missing.
}

static void validate_header_value(Request & request, std::string & field_name, std::string & field_value)
{
	if (field_name == "HOST")
		return validate_host(request, field_value);

	trim_white_spaces(request, field_value, " \t\v\f\b", "\n\r");

	if (field_name == "CONNECTION")
		validate_connection(request, field_value);

	if (request.get_method() == "POST")
	{
		if (field_name == "CONTENT_LENGTH")
		{
			if (request.isChunked())
				request.markAsBad(33);
			validate_content_length(request, field_value);
		}
		else if (field_name == "TRANSFER_ENCODING")
		{
			if (request.ContentLengthIsSet())
				request.markAsBad(33);
			if (!request.isChunked())
				validate_te(request, field_value);
		}
		else if (field_name == "CONTENT_TYPE")
			validate_content_type(request, field_value);
	}

	request.setHeader(field_name, field_value);
}

static bool     contain_unallowed_char(std::string & field_name)
{
	for (size_t i = 0; i < field_name.length(); i++)
	{
		if ((field_name[i] < 'a' || field_name[i] > 'z')
			&& (field_name[i] < 'A' || field_name[i] > 'Z')
			&& (field_name[i] != '_')
			&& (field_name[i] != '-'))
		{
			return true;
		}
		if (field_name[i] == '-')
			field_name[i] = '_';
	}
	return false;
}

static void    process_header(Request & request, std::string & header)
{
	std::string field_name;
	std::string field_value;
	size_t      colon_pos;

	colon_pos = header.find(':');
	if (colon_pos == std::string::npos)
		request.markAsBad(6);

	field_name = header.substr(0, colon_pos);
	if (field_name.empty() || contain_unallowed_char(field_name))
	{
		request.markAsBad(7888);
	}

	normalize_header_name(field_name);

	header.erase(0, colon_pos + 1);
	field_value = header;

	validate_header_value(request , field_name, field_value);
}

void    parse_headers(Request & request, std::string & msg)
{
	std::string         line;
	size_t              crlf_pos;

	crlf_pos = msg.find(CRLF);
	while (crlf_pos != std::string::npos)
	{
		line = msg.substr(0, crlf_pos);
		if (line.empty())
		{
			request.markHeadersParsed(true);
			msg.erase(0, 2);
			break ;
		}
		process_header(request, line);
		msg.erase(0, crlf_pos + 2);
		crlf_pos = msg.find(CRLF);
	}
}
