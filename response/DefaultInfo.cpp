
# include "DefaultInfo.hpp"

DefaultInfo::DefaultInfo()
{
    init_pages() ;
}

void DefaultInfo::init_pages(void) {
    error_pages[400] = BAD_REQUEST;
    error_pages[401] = UNAUTHORIZED;
    error_pages[403] = FORBIDDEN;
    error_pages[404] = NOT_FOUND;
    error_pages[405] = METHOD_NOT_ALLOWED ;
    error_pages[406] = NOT_ACCEPTABLE;

    error_pages[408] = REQUEST_TIMEOUT ;

    error_pages[411] = LENGTH_REQUIRED;
    error_pages[412] = PRECONDITION_FAILED;
    error_pages[413] = PAYLOAD_TOO_LARGE;
    error_pages[414] = URI_TOO_LONG ;

    error_pages[421] = MISDIRECTED_REQUEST ;

    error_pages[428] = PRECONDITION_REQUIRED ;

    error_pages[431] = REQUEST_HEADER_FIELDS_TOO_LARGE ;


    // Server error responses
    error_pages[500] = INTERNAL_SERVER_ERROR;
    error_pages[502] = BAD_GATEWAY ;
    error_pages[504] = GATEWAY_TIMEOUT ;
    error_pages[505] = HTTP_VERSION_NOT_SUPPORTED ;
    error_pages[508] = LOOP_DETECTED ;
    // Redirection messages
    error_pages[300] = MULTIPLE_CHOICES ;
    error_pages[301] = MOVED_PERMANENTLY ;
    error_pages[302] = FOUND ;
    error_pages[303] = SEE_OTHER ;
    error_pages[304] = NOT_MODIFIED ;
    error_pages[307] = TEMPORARY_REDIRECT ;
    error_pages[308] = PERMANENT_REDIRECT ;

    // Successful responses
    error_pages[200] = OK;
    error_pages[201] = CREATED ;
    error_pages[202] = ACCEPTED ;
    error_pages[203] = NON_AUTHORITATIVE_INFORMATION ;
    error_pages[204] = NO_CONTENT ;

}

std::string  DefaultInfo::getDefaultPage( const std::string & error_message )
{
    std::string html;

    html.append ("<!DOCTYPE>\n<html>\n<head><title>") ;
    html.append( error_message ) ;
    html.append( "</title></head>\n<body>\n<center><h1>" ) ;
    html.append( error_message ) ;
    html.append( "</h1></center>\n<hr><center>MyServer</center>\n</body>\n</html>\n" );

    return ( html ) ;
}

std::string  DefaultInfo::getCodeMsg( unsigned short error )
{
    std::map<short , std::string>::iterator it = this->error_pages.find(error);
    if ( it == this->error_pages.end() )
    {
        std::stringstream ss;
        ss << error;
        return ( ss.str() );
    }
    return ( it->second );
}

DefaultInfo::~DefaultInfo()
{
}
