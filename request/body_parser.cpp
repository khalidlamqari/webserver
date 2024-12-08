/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   body_parser.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 16:59:44 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/08 09:54:12 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_parse.hpp"
#include <unistd.h>

static char hex_to_char(const std::string & hex_code, Request & request)
{
    static const std::string    hex_chars = "0123456789ABCDEF";
    // static const std::string    hex_chars = "0123456789ABCDEF";
    // static const std::string    hex_chars = "0123456789ABCDEF";
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

static bool     contain_unallowed_char(const std::string & field_name)
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

static void normalize_header_name(std::string & name)
{
    for (size_t i = 0; i < name.length(); i++)
        name[i] = std::toupper(name[i]);
}

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

void trim_content_to_boundary_segment(const std::string & boundary, std::string & content)
{
    while (content.length())
    {
        size_t	carriageReturnPos = content.find('\r');
        size_t		i = 0;

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

static void	remove_ignored_area(const std::string & boundary, std::string & content)
{
	size_t		boundary_pos;
	std::string	dashed_boundary_crlf = boundary;

	dashed_boundary_crlf.insert(0, "--");
	dashed_boundary_crlf.append(CRLF);
	boundary_pos = content.find(dashed_boundary_crlf);

	// In this case the boundary is the first thing in the request body, else, the boundary might come after some useless data.
	if (!boundary_pos)
		return ;
	
	dashed_boundary_crlf.insert(0, CRLF);
	boundary_pos = content.find(dashed_boundary_crlf);

	if (boundary_pos != std::string::npos)
    {
		content.erase(0, boundary_pos); // TODO : Did not erased crlf
    }
	else
	{
		content.erase(0, content.length() - dashed_boundary_crlf.length());
		trim_content_to_boundary_segment(dashed_boundary_crlf, content);
	}
}

static std::string	validate_file_name(Request & request, const std::string & value)
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

static void validate_header_value(Request & request, std::string & field_value, std::string & file_name)
{
	std::stringstream	strm(field_value);
	std::string			param;
	int					i = 0;
	bool				name_param_found = false; // TODO : use the name param 

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
			{
				std::cout << "{" << param << "}"  << std::endl;
				request.markAsBad(932);
			}
		}
		i++;
	}
}

static void	check_part_header(Request & request, std::string & header, t_part & part)
{
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
    	validate_header_value(request , field_value, part.file_name); // ONLY VALIDATE THE HEADER IF IT S THE ONE WE ARE INTERESTED ON.
}

static void	extract_part_headers(Request & request, t_part & part, std::string & content)
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

static bool	is_valid_condidate_line(const std::string & boundary, std::string & content)
{
	// have to check for the final boundary as well!
	std::string dashed_boundary_crlf = boundary;

	dashed_boundary_crlf.insert(0, "--");
	dashed_boundary_crlf.insert(0, CRLF);
	dashed_boundary_crlf.append(CRLF);

	return (content.find(dashed_boundary_crlf) == 0);
}

static bool	is_possible_condidate_line(const std::string & boundary, std::string & content)
{
	std::string dashed_boundary_crlf = boundary;

	dashed_boundary_crlf.insert(0, "--");
	dashed_boundary_crlf.insert(0, CRLF);
	dashed_boundary_crlf.append(CRLF);

	return (dashed_boundary_crlf.find(content) == 0);
}

static bool	final_boundary_reached(const std::string & boundary, std::string & content)
{
	std::string close_boundary = boundary;	

	close_boundary.insert(0, "--"); 
	close_boundary.insert(0, CRLF);
	close_boundary.append("--");
	close_boundary.append(CRLF);

	return (content.find(close_boundary) == 0);
}

static void	extract_part_content(Request & request, t_part & part, std::string & content)
{
	size_t		crlf_pos;
	std::string	valid_data;

	crlf_pos = content.find(CRLF);
	while (crlf_pos != std::string::npos)
	{
		valid_data.append(content.substr(0, crlf_pos)); // TODO : check without saving in a string

		if (valid_data.find(request.get_boundary()) != std::string::npos)
			request.markAsBad(66);

		if (!part.file_name.empty())
		{
			if (part.file_content == NULL )
			{
				// request.get_ClientSocket()->response->getUploadDir() +
				// std::cout << "part.file_name " << part.file_name << std::endl;
				part.file_content = new std::ofstream(part.file_name, std::ios::out |std::ios::trunc | std::ios::binary);
			}
			*(part.file_content) << valid_data ; // this will be changed by writing directly at the file.
			// std::cout << "valid_data " << valid_data << std::endl;
		}

		valid_data.clear();
		content.erase(0, crlf_pos);

		if (final_boundary_reached(request.get_boundary(), content))
		{
			part.is_complete = true;
			request.markLastPartAsReached();
			request.markBodyParsed(true); // TODO : theire might be some data after the last boundary, and might have to be rood from the socket!
			content.clear();
			return ;
		}
		else if (is_valid_condidate_line(request.get_boundary(), content))
		{
			content.erase(0, 2);
			part.is_complete = true;
			return ;
		}
		else if (is_possible_condidate_line(request.get_boundary(), content))
		{
			part.unparsed_bytes = content;
			content.clear();
			return ;
		}
		else
		{
			*(part.file_content) << CRLF;
			content.erase(0, 2);
		}
		crlf_pos = content.find(CRLF);
	}
	
	// Breaking the loop means that some data that does not contain CRLF was found.
	if (content.find(request.get_boundary()) != std::string::npos)
			return request.markAsBad(66);

	if (!part.file_name.empty())
		*(part.file_content) << content;
	content.clear();
}

static void	extract_part(Request & request, std::string & content)
{
	// request.get_ClientSocket()->response->getUploadDir() ;
	t_part &			latest_part = request.get_latest_part(); // TODO : i gotta drop the onse that are not files!
	std::string			dashed_boundary_crlf;

	dashed_boundary_crlf = request.get_boundary();
	dashed_boundary_crlf.insert(0, "--");
	dashed_boundary_crlf.append("\r\n");

	content.insert(0, latest_part.unparsed_bytes);
	latest_part.unparsed_bytes.clear();

	// if it s new that means that in the previous part i found a crlf followed by a dashed_delim_crlf
	if (latest_part.is_new)
	{
		if (!content.find(dashed_boundary_crlf))
		{
			content.erase(0, dashed_boundary_crlf.length());
			latest_part.is_new = false;
		}
		else
		{ // this means that for example we just got a part of the delim and not all of it.
			latest_part.unparsed_bytes = content;
			content.clear();
			return ;
		}
	}

	if (!latest_part.header_parsed)
		extract_part_headers(request, latest_part, content);
	
	// when getting here the file name might or might not be set, only post the file if it s set! because that will be the only case where the part is related to a file input.
	// if it s a file then directly open it to start writing at it!
	if (latest_part.header_parsed)
	{
		extract_part_content(request, latest_part, content);
	}
	else
	{
		latest_part.unparsed_bytes = content; // in case we only got a part of a the headers not all of them.
		content.clear();
	}

	if (latest_part.is_complete && latest_part.file_name.empty())
	{
		std::cout << "last_part droped" << std::endl;	
		request.drop_last_part();
	}
	if (latest_part.is_complete && !latest_part.file_name.empty())
	{
		if (latest_part.file_content)
			latest_part.file_content->close();
	}
}

// Processes a single chunk, doesn t matter if the request is chunked or not, if it s not it just assumes that there is only one signle chunk.
static void	process_chunck(Request & request, std::string & chunk_content)
{
	if (request.isMultipart())
	{
		if (!request.hasReachedFirstPart())
		{
			remove_ignored_area(request.get_boundary(), chunk_content);
			if (chunk_content.empty())
				return ;
			request.markFirstPartAsReached();
		}

		if (!request.hasReachedLastPart())
		{
			while (!chunk_content.empty() && !request.hasReachedLastPart())
			{
				extract_part(request, chunk_content);
			}
		}
	}
	else
		chunk_content.clear();

}

size_t	hex_to_size_t(Request & request, const std::string & chunk_size)
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

void	update_chunk_state(Request & request, size_t length_left)
{
	if (!length_left)
		request.markAsHasUndoneChunk(false, 0);
	else
		request.markAsHasUndoneChunk(true, length_left);
}

static size_t	extract_chunk_length(Request & request, std::string & msg)
{
	size_t	chunk_length;
	size_t	crlf_pos;

	crlf_pos = msg.find(CRLF);
	if (crlf_pos == std::string::npos)
	{
		if (msg.length() >= 18)
			request.markAsBad(82);
		else
		{
			request.storeUnparsedMsg(msg);
			msg.clear();
			return 0;
		}
	}

	chunk_length = hex_to_size_t(request, msg.substr(0, crlf_pos));

	request.set_total_chunks_length(chunk_length);

	msg.erase(0, crlf_pos + 2);
	if (!chunk_length)
		request.markBodyParsed(true);

	return chunk_length;
}

std::string	find_chunk_content(Request & request, std::string & msg)
{
	std::string	chunk_content;
	size_t		chunk_length;

	chunk_length = request.hasAnUndoneChunk();

	if (!chunk_length && !request.isChunked())
		chunk_length = request.getContentLength();

	if (!chunk_length && request.isChunked())
	{
		if ((msg.length() >= 2) && request.get_total_chunks_length() && (msg.find(CRLF) != 0))
			request.markAsBad(499);

		if (msg.length() >= 2 && request.get_total_chunks_length())
			msg.erase(0, 2);

		chunk_length = extract_chunk_length(request, msg);
		if (!chunk_length)
			return "";
	}

	chunk_content = msg.substr(0, chunk_length);
	update_chunk_state(request, chunk_length - chunk_content.length());
	msg.erase(0, chunk_length);
	return chunk_content;
}

static void write_to_cgi_input(Request & request, std::string & content )
{
	write(request.get_ClientSocket()->response->get_pair_fds()[0],  content.c_str(), content.length());
}

void    parse_body(Request & request, std::string & msg)
{
	std::string	chunk_content;

	chunk_content = find_chunk_content(request, msg);
	while (!chunk_content.empty() && !request.hasParsedBody())
	{
		if ( request.get_ClientSocket()->response->is_cgi() )
		{
			write_to_cgi_input( request, chunk_content );
			// close(request.get_ClientSocket()->response->get_pair_fds()[0]);
			request.markBodyParsed(true);
		}
		else
			process_chunck(request, chunk_content);
		chunk_content.clear();
		chunk_content = find_chunk_content(request, msg);
	}
	if (request.hasParsedBody())
		request.markAsReady(true);
}
