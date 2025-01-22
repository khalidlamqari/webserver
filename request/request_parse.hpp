
#ifndef REQUEST_PARSE_HPP
# define REQUEST_PARSE_HPP

#include <string>
#include <cmath>
#include "Request.hpp"
#include "../Contexts/HttpContext.hpp"

#define CR      '\r'
#define LF      '\n'
#define CRLF    "\r\n"

void    parse_start_line(Request & request, const std::string & start_line);
void    parse_headers(Request & request, std::string & msg);
void    parse_body(Request & request, std::string & msg );

#endif