/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExecuteCgi.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 19:31:45 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/16 06:37:27 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"
# include <unistd.h>
#include <cstdlib>
#include <string>
#include <stdio.h>  

extern char **environ;

void    Response::setup_environment()
{
    std::map< std::string , std::vector< std::string > > headers = this->request.get_request_headers() ;

    std::map< std::string , std::vector< std::string > >::iterator it ;
    
    // setenv("AUTH_TYPE", "", 1);
    
    it = headers.find("Content-Length");
    if ( it != headers.end() && setenv("CONTENT_LENGTH", (*(it->second.begin())).c_str() , 1) == -1 )
        exit(1) ;

    it = headers.find("CONTENT_TYPE");
    if ( it != headers.begin() && setenv("CONTENT_TYPE", (*(it->second.begin())).c_str() , 1) == -1)
        exit(1);



    // setenv("QUERY_STRING", "?", 1);
    
    setenv("REQUEST_METHOD", this->request.get_request_method().c_str(), 1);
    // setenv("SCRIPT_NAME", "", 1);
    // setenv("SERVER_NAME", "", 1);
    // setenv("SERVER_PORT", "", 1);
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);

}

void Response::execute_cgi( void )
{
    std::cout << "path : " << this->_path_ << std::endl;

    char *av[] = {
        (char *)"/usr/bin/php",
        (char *)this->_path_.c_str(),
        NULL,
    };

    if ( socketpair(AF_UNIX, SOCK_STREAM, 0, this->s_fds) == -1 )
        throw 500 ;

    this->pid = fork();
    if ( this->pid == 0 )
    {
        if ( close(this->s_fds[0])  == -1 )
            exit(1);
        this->setup_environment() ;
        if ( dup2(this->s_fds[1], 1) == -1 )
            exit(1);
        execve(av[0], av, environ) ;
        exit(1);
    }
    else
    {
        waitpid(this->pid, NULL, 0) ;
        this->exit_stat = 0;
    }
}
