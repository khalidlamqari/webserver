
#ifndef UTILS_HPP
# define UTILS_HPP

// #include "../webserv.hpp"
#include <map>

#include <vector>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <map>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sstream>
#include <exception>

const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string GREEN = "\033[32m";
const std::string CYAN = "\033[36m";
const std::string RED = "\033[31m";

bool    is_space(char c);
void    err_throw( const char * message );
bool    is_all_digits(const std::string& str);
void	*ft_memset(void *b, int c, size_t len);
long    my_stoul(const std::string & str);
int		my_stoi(const std::string & str);
#endif
