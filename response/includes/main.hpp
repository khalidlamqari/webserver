
#pragma once

#ifndef MAIN_HPP
#define MAIN_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <poll.h>
#include <fcntl.h>
#include <deque>
#include "macros.hpp"
# include <sys/stat.h>
# include <dirent.h>
#include <ctime>
#include "../../Request/Request.hpp"
#include "../../server_setup/KqueueIdent.hpp"
# include "../../Contexts/HttpContext.hpp"
#include "../Response.hpp"
#include "../../Utils/utils.hpp"

#define RESP_BUFF 1024
class Response;
class ClientSocket;
static size_t num_file;



std::vector<std::string> split(const std::string & str);
bool                    is_all_WS( const std::string & str );
std::vector<std::string> _split_(const std::string & str, char c);
bool                    is_dir( const std::string & path );
bool                    is_file(std::string path);

std::string             get_rand_file_name(size_t & file_num);
void                    normalize_target( std::string &target, unsigned short & status);
std::string             get_content_type( const std::string & file_name );
const std::string       get_time(time_t sec);
bool                    is_dir( const std::string & path );
bool                    is_file(std::string path);
bool                    is_existe(const std::string & path);
void                    check_is_cgi( Response & response );

void                    create_html_table(std::string & ls_files, const std::string & target);
void                    append_row( const Response & response, struct dirent * f, std::string & ls_files );
void                    set_cgi_requerements( Response & response, bool & is_cgi);
const ServerContext *   get_server_context(ClientSocket & client_info);
void                    extract_info_from_location(Response & response, LocationContext & location);
void                    extract_info_from_server(Response & response,  const ServerContext & servercontext);
void                    remove_last_slash( std::string & target );
void                    set_connection_header(Request & request, std::string & message, unsigned short & status);
bool                    is_directory_list(const std::string & path, LocationContext * location, const ServerContext & servercontext);
void                    open_file(const bool & tranfer_encoding, std::ifstream & page_content, const std::string & path);
void                    read_file(std::ifstream & page_content, char *buffer, size_t & size);
void                    check_end_of_file(std::ifstream & page_content, bool & end_of_response, bool & tranfer_encoding );
std::string             extract_headers( std::string & unparsed_content );
std::string             extract_body(const std::string & unparsed_content, size_t pos);
bool                    is_redirect(LocationContext * location);
void                    remove_file(const std::string & path);
void                    remove_dir_recursive(const std::string & path);
std::vector<std::string> split(const std::string & str, const std::string & to_find );
void                    write_headers_to_msg(const std::vector<std::string> & headers, std::string & message);
std::string             get_header_value(const std::vector<std::string> & headers, const std::string & header_name, bool & is_set);
bool                    is_name_in_header(std::string header, std::string header_name);
void                    to_upper(std::string & str);
void                    trim_white_spaces(std::string & header_value, const std::string & allowedWS, const std::string & forbidenWS) ;
void    print_log(Response & response, bool desconnected);


#endif