/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 09:50:31 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/14 09:20:06 by klamqari         ###   ########.fr       */
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

#define BUFFER_SIZE 30000

#define MAX_CLIENTS 10000


std::vector<std::string> split(const std::string & str) ;
std::string trim(const std::string & str ) ;

#endif