
#include "HttpContext.hpp"

HttpContext::HttpContext( void )
{
    auto_index = false;
    max_body_size = 1000000;
    cgi_read_timeout = 0;

    auto_ind_is_set = false;
    max_body_is_set = false;
    cgi_read_timeout_is_set = false;
}

HttpContext::~HttpContext()
{
     
}

void    HttpContext::set_error_page( const t_error_page& error_page )
{
    std::vector<t_error_page>::iterator it;

    for (it = this->error_pages.begin(); it != this->error_pages.end(); it++)
    {
        if ((*it).err_code == error_page.err_code)
        {
            (*it).path = error_page.path;

            return ;
        }
    }

    this->error_pages.push_back(error_page);
}

void    HttpContext::set_auto_index(const std::string & on_off)
{
    if (on_off == "on")
        this->auto_index = true;
    else
        this->auto_index = false;
}

void    HttpContext::set_cgi_info(const std::pair<extension, execPath>& info)
{
    this->CgiExecPaths[info.first] = info.second;
}

void   HttpContext::set_new_server( void )
{
    ServerContext   new_server(CgiExecPaths);

    std::vector<t_error_page>::iterator it;

    /* Making the new server inherit the httpContext directives. */
    for (it = this->error_pages.begin(); it < this->error_pages.end(); it++)
        new_server.set_error_page(*it);

    if (this->auto_index)
        new_server.set_auto_index("on");
    else
        new_server.set_auto_index("off");
    
    new_server.set_max_body_size(max_body_size);
    new_server.set_cgi_rd_tm_out(cgi_read_timeout);
    
    /* Pushing the newly created server to the servers vector */
    this->servers.push_back(new_server);
}

void    HttpContext::set_max_body_size( size_t limit )
{
    this->max_body_size = limit;
}

void	HttpContext::set_cgi_rd_tm_out( time_t max_time )
{
    this->cgi_read_timeout = max_time;
}

ServerContext&  HttpContext::get_latest_server( void )
{
    return this->servers.back();
}

const std::vector<ServerContext>& HttpContext::get_servers( void ) const
{
    return this->servers;
}

const std::vector<t_error_page>& HttpContext::get_error_pages( void ) const
{
    return this->error_pages;
}

const std::map<extension, execPath>&  HttpContext::get_cgi_info( void ) const
{
    return this->CgiExecPaths;
}

const bool& HttpContext::get_auto_index( void ) const
{
    return this->auto_index;
}

const time_t&   HttpContext::get_cgi_read_time_out( void ) const
{
    return this->cgi_read_timeout;
}
