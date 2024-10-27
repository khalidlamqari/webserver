/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 09:50:31 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/27 12:45:46 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include <sstream>   // For std::istringstream
#include <map>
#include <poll.h>
#include <fstream>
#include <fcntl.h>
#include <deque>
# include "../config_file_parsing/HttpContext.hpp"


#define BUFFER_SIZE 1024

#define MAX_CLIENTS 1024


std::vector<std::string> split(const std::string & str) ;
std::string trim(const std::string & str ) ;
bool is_all_WS( const std::string & str ) ;
bool is_all_digit(const std::string & str) ;
#endif