/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macros.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 17:34:09 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/03 11:37:14 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#ifndef MACROS_HPP
# define MACROS_HPP

#define RESP_BUFF 1000 //(2^16)

// Informational responses
#define CONTINUE "100 Continue"
#define SWITCHING_PROTOCOLS "101 Switching Protocols"
#define PROCESSING "102 Processing"

// Successful responses
#define OK "200 OK"
#define CREATED "201 Created"
#define ACCEPTED "202 Accepted"
#define NON_AUTHORITATIVE_INFORMATION "203 Non-Authoritative Information"
#define NO_CONTENT "204 No Content"
#define RESET_CONTENT "205 Reset Content"
#define PARTIAL_CONTENT "206 Partial Content"
#define MULTI_STATUS "207 Multi-Status"
#define ALREADY_REPORTED "208 Already Reported"

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
#define PAYMENT_REQUIRED "402 Payment Required"
#define FORBIDDEN "403 Forbidden"
#define NOT_FOUND "404 Not Found"
#define METHOD_NOT_ALLOWED "405 Method Not Allowed"
#define NOT_ACCEPTABLE "406 Not Acceptable"
#define PROXY_AUTHENTICATION_REQUIRED "407 Proxy Authentication Required"
#define REQUEST_TIMEOUT "408 Request Timeout"
#define CONFLICT "409 Conflict"
#define GONE "410 Gone"
#define LENGTH_REQUIRED "411 Length Required"
#define PRECONDITION_FAILED "412 Precondition Failed"
#define PAYLOAD_TOO_LARGE "413 Payload Too Large"
#define URI_TOO_LONG "414 URI Too Long"
#define UNSUPPORTED_MEDIA_TYPE "415 Unsupported Media Type"
#define RANGE_NOT_SATISFIABLE "416 Range Not Satisfiable"
#define EXPECTATION_FAILED "417 Expectation Failed"
#define IM_A_TEAPOT "418 I'm a teapot" // Fun Easter egg status code
#define MISDIRECTED_REQUEST "421 Misdirected Request"
#define UNPROCESSABLE_ENTITY "422 Unprocessable Entity"
#define LOCKED "423 Locked"
#define FAILED_DEPENDENCY "424 Failed Dependency"
#define TOO_EARLY "425 Too Early"
#define UPGRADE_REQUIRED "426 Upgrade Required"
#define PRECONDITION_REQUIRED "428 Precondition Required"
#define TOO_MANY_REQUESTS "429 Too Many Requests"
#define REQUEST_HEADER_FIELDS_TOO_LARGE "431 Request Header Fields Too Large"
#define UNAVAILABLE_FOR_LEGAL_REASONS "451 Unavailable For Legal Reasons"

// Server error responses
#define INTERNAL_SERVER_ERROR "500 Internal Server Error"
#define NOT_IMPLEMENTED "501 Not Implemented"
#define BAD_GATEWAY "502 Bad Gateway"
#define SERVICE_UNAVAILABLE "503 Service Unavailable"
#define GATEWAY_TIMEOUT "504 Gateway Timeout"
#define HTTP_VERSION_NOT_SUPPORTED "505 HTTP Version Not Supported"
#define VARIANT_ALSO_NEGOTIATES "506 Variant Also Negotiates"
#define INSUFFICIENT_STORAGE "507 Insufficient Storage"
#define LOOP_DETECTED "508 Loop Detected"
#define NOT_EXTENDED "510 Not Extended"
#define NETWORK_AUTHENTICATION_REQUIRED "511 Network Authentication Required"


#endif

