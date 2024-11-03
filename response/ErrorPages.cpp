/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPages.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 15:21:34 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/03 16:16:34 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/main.hpp"

ErrorPages::ErrorPages()
{
    this->init_pages() ;
}

void ErrorPages::init_pages(void) {
    this->error_pages[400] = BAD_REQUEST;
    this->error_pages[401] = UNAUTHORIZED;
    this->error_pages[402] = PAYMENT_REQUIRED;
    this->error_pages[403] = FORBIDDEN;
    this->error_pages[404] = NOT_FOUND;
    this->error_pages[405] = METHOD_NOT_ALLOWED ;
    this->error_pages[406] = NOT_ACCEPTABLE;
    this->error_pages[407] = PROXY_AUTHENTICATION_REQUIRED ;
    this->error_pages[408] = REQUEST_TIMEOUT ;
    this->error_pages[409] = CONFLICT ;
    this->error_pages[410] = GONE ;
    this->error_pages[411] = LENGTH_REQUIRED;
    this->error_pages[412] = PRECONDITION_FAILED;
    this->error_pages[413] = PAYLOAD_TOO_LARGE;
    this->error_pages[414] = URI_TOO_LONG ;
    this->error_pages[415] = UNSUPPORTED_MEDIA_TYPE ;
    this->error_pages[416] = RANGE_NOT_SATISFIABLE;
    this->error_pages[417] = EXPECTATION_FAILED ;
    this->error_pages[418] = IM_A_TEAPOT;
    this->error_pages[421] = MISDIRECTED_REQUEST ;
    this->error_pages[422] = UNPROCESSABLE_ENTITY ;
    this->error_pages[423] = LOCKED ;
    this->error_pages[424] = FAILED_DEPENDENCY;
    this->error_pages[425] = TOO_EARLY ;
    this->error_pages[426] = UPGRADE_REQUIRED;
    this->error_pages[428] = PRECONDITION_REQUIRED ;
    this->error_pages[429] = TOO_MANY_REQUESTS ;
    this->error_pages[431] = REQUEST_HEADER_FIELDS_TOO_LARGE ;
    this->error_pages[451] = UNAVAILABLE_FOR_LEGAL_REASONS ;

    
    // Server error responses
    this->error_pages[500] = INTERNAL_SERVER_ERROR;
    this->error_pages[501] = NOT_IMPLEMENTED;
    this->error_pages[502] = BAD_GATEWAY ;
    this->error_pages[503] = SERVICE_UNAVAILABLE ;
    this->error_pages[504] = GATEWAY_TIMEOUT ;
    this->error_pages[505] = HTTP_VERSION_NOT_SUPPORTED ;
    this->error_pages[506] = VARIANT_ALSO_NEGOTIATES ;
    this->error_pages[507] = INSUFFICIENT_STORAGE ;
    this->error_pages[508] = LOOP_DETECTED ;
    this->error_pages[510] = NOT_EXTENDED ;
    this->error_pages[511] = NETWORK_AUTHENTICATION_REQUIRED ;

    // Redirection messages
    this->error_pages[300] = MULTIPLE_CHOICES ;
    this->error_pages[301] = MOVED_PERMANENTLY ;
    this->error_pages[302] = FOUND ;
    this->error_pages[303] = SEE_OTHER ;
    this->error_pages[304] = NOT_MODIFIED ;
    this->error_pages[307] = TEMPORARY_REDIRECT ;
    this->error_pages[308] = PERMANENT_REDIRECT ;

    // Successful responses
    this->error_pages[200] = OK;
    this->error_pages[201] = CREATED ;
    this->error_pages[202] = ACCEPTED ;
    this->error_pages[203] = NON_AUTHORITATIVE_INFORMATION ;
    this->error_pages[204] = NO_CONTENT ;
    this->error_pages[205] = RESET_CONTENT ;
    this->error_pages[206] = PARTIAL_CONTENT ;
    this->error_pages[207] = MULTI_STATUS ;
    this->error_pages[208] = ALREADY_REPORTED ;
    
}

/*
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head><title>504 Gateway Timeout</title></head>\n"
    "<body>\n"
    "<center><h1>504 Gateway Timeout</h1></center>\n"
    "<hr><center>MyServer</center>\n"
    "</body>\n"
    "</html>\n";
*/

std::string  ErrorPages::getErrorPage( const std::string & error_message )
{
    std::string html;

    html.append ("<!DOCTYPE html>\n<html>\n<head><title>") ;
    html.append( error_message ) ;
    html.append( "</title></head>\n<body>\n<center><h1>" ) ;
    html.append( error_message ) ;
    html.append( "</h1></center>\n<hr><center>MyServer</center>\n</body>\n</html>\n" );
    
    return ( html ) ;
}

const std::string & ErrorPages::getErrorMsg( unsigned short error )
{
    return ( this->error_pages.find( error )->second ) ;
}

ErrorPages::~ErrorPages()
{
    
}
