
# pragma once

#ifndef MACROS_HPP
# define MACROS_HPP




// Successful responses
#define OK "200 OK"
#define CREATED "201 Created"
#define ACCEPTED "202 Accepted"
#define NON_AUTHORITATIVE_INFORMATION "203 Non-Authoritative Information"
#define NO_CONTENT "204 No Content"

// Redirection messages
#define MULTIPLE_CHOICES "300 Multiple Choices"
#define MOVED_PERMANENTLY "301 Moved Permanently"
#define FOUND "302 Found"
#define SEE_OTHER "303 See Other"
#define NOT_MODIFIED "304 Not Modified"
#define TEMPORARY_REDIRECT "307 Temporary Redirect"
#define PERMANENT_REDIRECT "308 Permanent Redirect"

// Client error responses
#define BAD_REQUEST "400 Bad Request"
#define UNAUTHORIZED "401 Unauthorized"

#define FORBIDDEN "403 Forbidden"
#define NOT_FOUND "404 Not Found"
#define METHOD_NOT_ALLOWED "405 Method Not Allowed"
#define NOT_ACCEPTABLE "406 Not Acceptable"

#define REQUEST_TIMEOUT "408 Request Timeout"

#define LENGTH_REQUIRED "411 Length Required"
#define PRECONDITION_FAILED "412 Precondition Failed"

#define PAYLOAD_TOO_LARGE "413 Payload Too Large"
#define URI_TOO_LONG "414 URI Too Long"
#define REQUEST_HEADER_FIELDS_TOO_LARGE "431 Request Header Fields Too Large"
#define MISDIRECTED_REQUEST "421 Misdirected Request"

#define PRECONDITION_REQUIRED "428 Precondition Required"

// Server error responses
#define INTERNAL_SERVER_ERROR "500 Internal Server Error"
#define BAD_GATEWAY "502 Bad Gateway"

#define GATEWAY_TIMEOUT "504 Gateway Timeout"
#define HTTP_VERSION_NOT_SUPPORTED "505 HTTP Version Not Supported"

#define LOOP_DETECTED "508 Loop Detected"



#endif

