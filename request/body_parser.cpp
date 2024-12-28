/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   body_parser.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 16:59:44 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/28 10:08:57 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_parse.hpp"
#include <unistd.h>

static char hex_to_char(const std::string & hex_code, Request & request) // ok
{
    static const std::string    hex_chars = "0123456789ABCDEF";
	int							value;
    char first = std::toupper(hex_code[0]);
    char second = std::toupper(hex_code[1]);

    if ((first == '0' && second == '0')
        || (hex_chars.find(first) == std::string::npos)
        || (hex_chars.find(second) == std::string::npos)) {
        request.markAsBad(9);
    }

	value = 16 * hex_chars.find(first) + hex_chars.find(second);
	if (value > 127)
		request.markAsBad(9);
    return (value);
}

static bool     contain_unallowed_char(const std::string & field_name) // ok
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
    }
    return false;
}

static void normalize_header_name(std::string & name) // ok
{
    for (size_t i = 0; i < name.length(); i++)
        name[i] = std::toupper(name[i]);
}

static void trim_white_spaces(Request & request, std::string & header_value, const std::string & allowedWS, const std::string & forbidenWS) // ok
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

void trim_content_to_boundary_segment(const std::string & boundary, std::string & content) // ok
{
    while (content.length())
    {
        size_t	carriageReturnPos = content.find('\r');
        size_t	i = 0;

        if (carriageReturnPos == std::string::npos)
        {
            content.clear();
            return ;
        }
        content.erase(0, carriageReturnPos);
        while (i != content.length() && content[i] == boundary[i])
            i++;
        
        if (i == content.length())
            return ;
        
        content.erase(0, i);
    }
}

// Removing all the data before the first boundary
static void	remove_ignored_area(const std::string & boundary, std::string & content) // ok
{
	size_t		boundary_pos;
	std::string	dashed_boundary_crlf = boundary;

	dashed_boundary_crlf.insert(0, "--");
	dashed_boundary_crlf.append(CRLF);
	boundary_pos = content.find(dashed_boundary_crlf);

	if (!boundary_pos)
		return ;

	dashed_boundary_crlf.insert(0, CRLF);
	boundary_pos = content.find(dashed_boundary_crlf);

	if (boundary_pos != std::string::npos)
    {
		content.erase(0, boundary_pos + 2);
    }
	else
	{
		if (content.length() >= dashed_boundary_crlf.length())
			content.erase(0, content.length() - dashed_boundary_crlf.length());
		trim_content_to_boundary_segment(dashed_boundary_crlf, content);
	}
}

static std::string	validate_file_name(Request & request, const std::string & value) // ok
{
	std::string                 file_name;
    size_t                      value_length = value.length();

	// TODO : MIGHT HAVE TO CHECK THE LENGTH AS WELL!

    for (size_t i = 1; i < value_length - 1; i++)
    {
		if (value[i] == '/')
			file_name += '_';
        else if (value[i] != '%')
            file_name += value[i];
        else if (i <= value_length - 3)
        {
            file_name += hex_to_char(value.substr(i + 1, 2), request);
            i += 2;
        }
        else
            request.markAsBad(10);
    }
    return file_name;
}

static void validate_header_value(Request & request, std::string & field_value, std::string & file_name) // ok
{
	std::stringstream	strm(field_value);
	std::string			param;
	int					i = 0;
	bool				name_param_found = false;

	while (std::getline(strm, param, ';'))
	{
		trim_white_spaces(request, param, " \t\v\f\b", "\n\r");
		if (i == 0 && param != "form-data")
			return ;
		if (param.find("name=") == 0)
		{
			if ((param.length() >= 7) && (param[5] == '"') && (param.back() == '"'))
				name_param_found = true;
			else
				request.markAsBad(931);
		}
		if (param.find("filename=") == 0)
		{
			if ((param.length() > 11) && (param[9] == '"') && (param.back() == '"'))
				file_name = validate_file_name(request, param.substr(9));
			else
				request.markAsBad(932);
		}
		i++;
	}

	if (!file_name.empty() && !name_param_found)
		request.markAsBad(933);
}

static void	check_part_header(Request & request, std::string & header, t_part & part) // ok
{
	std::cout << header << std::endl;
	std::string field_name;
    std::string field_value;
    size_t      colon_pos;

    colon_pos = header.find(':');
    if (colon_pos == std::string::npos)
        request.markAsBad(6);

    field_name = header.substr(0, colon_pos);
    if (field_name.empty() || contain_unallowed_char(field_name))
        request.markAsBad(7);

    normalize_header_name(field_name);

    header.erase(0, colon_pos + 1);
    field_value = header;

	if ((field_name == "CONTENT-DISPOSITION") && (field_value.find("filename=") != std::string::npos))
    	validate_header_value(request , field_value, part.file_name);

	if (!part.file_name.empty() && !part.file_opened)
	{
		part.file = new std::ofstream(part.file_name, std::ios::out | std::ios::trunc | std::ios::binary);

		if (!part.file || !part.file->is_open())
			request.markAsBad(500);
		part.file_opened = true;
	}
}

static void	extract_part_headers(Request & request, t_part & part, std::string & content) // ok
{
	size_t				crlf_pos;
	std::string			header;

	crlf_pos = content.find(CRLF);
    while (crlf_pos != std::string::npos)
	{
		header = content.substr(0, crlf_pos);
		if (header.empty())
		{
			part.header_parsed = true;
			content.erase(0, 2); // removing the crlf that splits the headers from the rest of the part
			return ;
		}
		if (header.find(request.get_boundary()) != std::string::npos)
			request.markAsBad(776);
		check_part_header(request, header, part);
		content.erase(0, crlf_pos + 2);
		crlf_pos = content.find(CRLF);
	}
}

static bool	is_valid_condidate_line(const std::string & boundary, std::string & content) // ok
{
	return (content.find(boundary) == 0);
}

static bool	is_possible_condidate_line(const std::string & boundary, std::string & content) // ok
{
	std::string	final_boundary = boundary;
	
	final_boundary.insert(boundary.length() - 2, "--");
	return ((boundary.find(content) == 0) || (final_boundary.find(content) == 0));
}

static bool	final_boundary_reached(const std::string & boundary, std::string & content) // ok
{
	return (content.find(boundary) == 0);
}

static void	extract_part_content(Request & request, t_part & part, std::string & content) // ok
{
	size_t		crlf_pos;
	size_t		valid_length = 0;
	std::string	valid_data;

	crlf_pos = content.find(CRLF);
	while (crlf_pos != std::string::npos)
	{
		valid_data.append(content.substr(0, crlf_pos));

		if (valid_data.find(request.get_boundary(), valid_length) != std::string::npos)
			request.markAsBad(66);

		valid_length += valid_data.length();

		content.erase(0, crlf_pos);

		if (final_boundary_reached(request.build_boundary(2), content))
		{
			part.is_complete = true;
			request.markLastPartAsReached();
			request.markBodyParsed(true);
			content.clear();

			if (!part.file_name.empty())
			{
				*(part.file) << valid_data;
				part.file->close();
			}
			return ;
		}
		else if (is_valid_condidate_line(request.build_boundary(3), content))
		{
			content.erase(0, 2);
			part.is_complete = true;

			if (!part.file_name.empty())
			{
				*(part.file) << valid_data;
				part.file->close();	
			}
			return ;
		}
		else if (is_possible_condidate_line(request.build_boundary(3), content))
		{
			part.unparsed_bytes = content;
			content.clear();

			if (!part.file_name.empty())
				*(part.file) << valid_data;
			return ;
		}
		else
		{
			valid_data.append(CRLF);
			valid_length += 2;
			content.erase(0, 2);
		}
		crlf_pos = content.find(CRLF);
	}

	if (content.find(request.get_boundary()) != std::string::npos)
		request.markAsBad(66);

	valid_data.append(content);
	content.clear();

	if (!part.file_name.empty())
		*(part.file) << valid_data;
}
 
static void	extract_part(Request & request, std::string & content) // ok
{
	t_part &			latest_part = request.get_latest_part();

	std::string			boundary = request.build_boundary(1);

	content.insert(0, latest_part.unparsed_bytes);
	latest_part.unparsed_bytes.clear();

	if (latest_part.is_new)
	{
		if (!content.find(boundary))
		{
			content.erase(0, boundary.length());
			latest_part.is_new = false;
		}
		else
		{
			latest_part.unparsed_bytes = content;
			content.clear();
			return ;
		}
	}

	if (!latest_part.header_parsed)
		extract_part_headers(request, latest_part, content);

	if (!latest_part.header_parsed)
	{
		latest_part.unparsed_bytes = content;
		content.clear();
		return ;
	}

	extract_part_content(request, latest_part, content);

	if (latest_part.is_complete && latest_part.file_name.empty())
		request.drop_last_part();
}

static void	process_chunck(Request & request, std::string & chunk_content)
{
	if (request.isMultipart()) // TODO : && the request target is not CGI
	{
		if (!request.hasReachedFirstPart())
		{
			remove_ignored_area(request.get_boundary(), chunk_content);
			if (chunk_content.empty())
				return ;
			else if (chunk_content.length() < request.get_boundary().length() + 4)	// -- + boundary + CRLF
			{
				request.total_body_length -= chunk_content.length();
				return request.storeUnparsedMsg(chunk_content);
			}
			request.markFirstPartAsReached();
		}

		if (!request.hasReachedLastPart())
		{
			while (!chunk_content.empty() && !request.hasReachedLastPart())
			{
				extract_part(request, chunk_content);
			}
		}
		else
			chunk_content.clear();
	}
	else
		chunk_content.clear();
}

size_t	hex_to_size_t(Request & request, const std::string & chunk_size) // ok
{
	std::string valid_hex_chars = "0123456789ABCDEF";
    size_t      size = 0;

    if (chunk_size[0] == '0' && chunk_size.length() != 1)
    {
        request.markAsBad(221);
    }

    if ((chunk_size.length() > 16) || (chunk_size.length() == 16 && chunk_size > "FFFFFFFFFFFFFFFF"))
    {
        request.markAsBad(222);
    }

    for (size_t i = 0; i < chunk_size.length(); i++)
    {
        if (valid_hex_chars.find(chunk_size[i]) == std::string::npos)
        {
            request.markAsBad(223);
        }
        else
            size += (size_t) (valid_hex_chars.find(std::toupper(chunk_size[i])) * pow(16, (chunk_size.length() - i - 1)));
    }
    return size;
}

void	update_chunk_state(Request & request, size_t length_left) // ok
{
	request.markAsHasUndoneChunk(length_left);
}

static std::string	extract_chunk_length(Request & request, std::string & msg) // ok
{
	size_t		crlf_1_pos;
	size_t		crlf_2_pos;
	std::string	length;

	crlf_1_pos = msg.find(CRLF);

	if (crlf_1_pos != 0)
	{
		if ((crlf_1_pos != std::string::npos) || (msg.length() >= 2))
			request.markAsBad(499);
	}

	crlf_2_pos = msg.find(CRLF, 2);
	if (crlf_2_pos == std::string::npos)
	{
		if (msg.length() >= 20)				// CRLF FFFFFFFFFFFFFFFF CRLF
			request.markAsBad(82);
	}
	else if (crlf_2_pos == 2)
		request.markAsBad(888);

	if (crlf_1_pos == std::string::npos || crlf_2_pos == std::string::npos)
	{
		request.storeUnparsedMsg(msg);
		msg.clear();
		return "";
	}

	msg.erase(0, 2);
	crlf_2_pos -= 2;

	length = msg.substr(0, crlf_2_pos);
	msg.erase(0, length.length() + 2);

	for (size_t i= 0; i < length.length(); i++)
		length[i] = std::toupper(length[i]);	
	return length;
}

static size_t	find_chunk_length(Request & request, std::string & msg) // ok
{
	size_t	chunk_length;

	std::string	length = extract_chunk_length(request, msg);

	if (length.empty())
		return 0;

	chunk_length = hex_to_size_t(request, length);

	request.total_body_length  += chunk_length;
	// TODO : compare it max body size! and compare the content length after reading headers.
	if (!chunk_length)
		request.markBodyParsed(true);

	return chunk_length;
}

std::string	find_chunk_content(Request & request, std::string & msg) // ok
{
	std::string	chunk_content;
	size_t		chunk_length;

	// In case a previous chunk is not yet fully read
	chunk_length = request.hasAnUndoneChunk();

	if (!chunk_length)
	{
		if (!request.isChunked())
		{
			if (request.total_body_length == request.getContentLength())
			{
				request.markBodyParsed(true);
				return "";
			}
			chunk_length = request.getContentLength();
		}
		else
		{
			if (!request.first_chunk_fixed)
			{
				msg.insert(0, CRLF);
				request.first_chunk_fixed = true;
			}
			chunk_length = find_chunk_length(request, msg);
		}
	}

	chunk_content = msg.substr(0, chunk_length);
	msg.erase(0, chunk_length);
	update_chunk_state(request, chunk_length - chunk_content.length());

	if (!request.isChunked())
		request.total_body_length += chunk_content.length();

	return chunk_content;
}

void    parse_body(Request & request, std::string & msg)
{
	std::string	chunk_content;

	chunk_content = find_chunk_content(request, msg);
	while (!chunk_content.empty() && !request.hasParsedBody())
	{
		chunk_content.insert(0, request.getUnparsedMsg());
		request.resetUnparsedMsg();
		process_chunck(request, chunk_content);
		chunk_content.clear();
		chunk_content = find_chunk_content(request, msg);
	}

	if (request.hasParsedBody())
	{
		request.markAsReady(true);
	}
}
