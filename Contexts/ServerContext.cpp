
#include "ServerContext.hpp"

ServerContext::ServerContext( const std::map<extension, execPath>& CgiExecPaths ) : CgiExecPaths(CgiExecPaths)
{
    this->index = "index.html"; // Setting "index.html" as default index.

    host_is_set = false;
    root_is_set = false;
    port_is_set = false;
    index_is_set = false;
    methods_is_set = false;
    upl_dir_is_set = false;
    auto_ind_is_set = false;
    srv_names_is_set = false;
    cgi_read_timeout_is_set = false;
    cgi_info_inherited = true;
}

ServerContext::~ServerContext()
{
    
}

void    ServerContext::set_error_page(const t_error_page& error_info)
{
    std::vector<t_error_page>::iterator it;

    for (it = this->error_pages.begin(); it != this->error_pages.end(); it++)
    {
        if ((*it).err_code == error_info.err_code)
        {
            (*it).path = error_info.path;
            return ;
        }
    }

    this->error_pages.push_back(error_info);
}

void   ServerContext::set_new_location( const std::string & location )
{
    LocationContext   new_location(location, CgiExecPaths);

    std::vector<t_error_page>::iterator it;

    /* Making the new location inherit the serverContext directives. */
    for (it = this->error_pages.begin(); it < this->error_pages.end(); it++)
        new_location.set_error_page(*it);

    new_location.set_allowed_methods(this->allowed_methods);
    new_location.set_root_directory(this->root_directory);
    new_location.set_upload_dir(this->upload_dir);
    new_location.set_index(this->index);

    if (this->auto_index)
        new_location.set_auto_index("on");
    else
        new_location.set_auto_index("off");

    this->locations.push_back(new_location);
}

void    ServerContext::set_ports( std::vector<unsigned short> ports )
{
    this->ports.assign(ports.begin(), ports.end());
}

void    ServerContext::set_root_directory( std::string root )
{
    this->root_directory = root;
}

void    ServerContext::set_upload_dir( std::string directory )
{
    this->upload_dir = directory;
}

void    ServerContext::set_index( std::string index )
{
    this->index = index;
}

void    ServerContext::set_server_names( std::vector<std::string> names )
{
    this->server_names.assign(names.begin(), names.end());
}

void    ServerContext::set_auto_index(const std::string & on_off)
{
    if (on_off == "on")
        this->auto_index = true;
    else
        this->auto_index = false;
}

void    ServerContext::set_allowed_methods( std::vector<std::string> methods )
{
    this->allowed_methods.assign(methods.begin(), methods.end());
}

void    ServerContext::set_host( std::string host )
{
    this->host = host;
}

void    ServerContext::set_max_body_size(const size_t & size)
{
    this->max_body_size = size;
}

void    ServerContext::set_cgi_info(const std::pair<extension, execPath>& info)
{
    if (cgi_info_inherited)
		clear_cgi_info();
    this->CgiExecPaths[info.first] = info.second;
}

void	ServerContext::set_cgi_rd_tm_out( time_t max_time )
{
    this->cgi_read_timeout = max_time;
}

/* Getters */

LocationContext&  ServerContext::get_latest_location( void )
{
    return this->locations.back();
}


const std::vector<t_error_page>& ServerContext::get_error_pages( void ) const
{
    return this->error_pages;
}

const std::vector<LocationContext>& ServerContext::get_locations( void ) const
{
    return this->locations;
}

const std::vector<std::string>& ServerContext::get_server_names( void ) const
{
    return this->server_names;
}

const std::vector<std::string>& ServerContext::get_allowed_methods( void ) const
{
    return this->allowed_methods;
}

const std::vector<unsigned short>& ServerContext::get_ports( void ) const
{
    return this->ports;
}

const std::map<extension, execPath>& ServerContext::get_cgi_info( void ) const
{
    return this->CgiExecPaths;
}

const std::string& ServerContext::get_root_directory( void ) const
{
    return this->root_directory;
}

const std::string& ServerContext::get_upload_dir( void ) const
{
    return this->upload_dir;
}

const std::string& ServerContext::get_index( void ) const
{
    return this->index;
}

const std::string& ServerContext::get_host( void ) const
{
    return this->host;
}

const bool& ServerContext::get_auto_index( void ) const
{
    return this->auto_index;
}

const size_t&   ServerContext::get_max_body_size( void ) const
{
    return this->max_body_size;
}

/* Methods */
void    ServerContext::clear_cgi_info( void )
{
    while (!CgiExecPaths.empty())
    {
        CgiExecPaths.erase(CgiExecPaths.begin());
    }
    cgi_info_inherited = false;
}

void    ServerContext::show_server_names( void ) const
{
    for (std::vector<std::string>::const_iterator it = server_names.begin(); it != server_names.end(); it++)
    {
        if (it != server_names.begin())
            std::cout << "\033[32m" << ", " << "\033[0m";
        std::cout << *it;
    }
}

const time_t&   ServerContext::get_cgi_read_time_out( void ) const
{
    return this->cgi_read_timeout;
}
