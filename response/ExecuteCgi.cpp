/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExecuteCgi.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 19:31:45 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/02 13:05:24 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"
# include <unistd.h>
#include <cstdlib>
#include <string>
#include <stdio.h>

/*

(char *)"REDIRECT_STATUS=1",
(char *)"SCRIPT_FILENAME=/path/to/your/script.php",
(char *)"QUERY_STRING=param1=value1&param2=value2",

*/



void    Response::setup_environment(char ***env)
{
    int size = this->request.get_headers().size() + 7;

    *env = (char **)malloc(sizeof(char *) * size );
    size = 0;
    std::string var;
    std::map<std::string, std::string> headers = this->request.get_headers() ;
    std::map< std::string , std::vector< std::string > >::iterator it ;
    var = "REQUEST_METHOD=" + this->request.get_request_method() ;
    (*env)[size++] = strdup( var.c_str() ) ;
    var = "QUERY_STRING=" ;
    (*env)[size++] = strdup( var.c_str() ) ;
    var = "PATH_INFO=" + this->_path_info ;
    (*env)[size++] = strdup( var.c_str() ) ;
    (*env)[size++] = strdup( "SERVER_PROTOCOL=HTTP/1.1" ) ;
    (*env)[size++] = strdup( "REDIRECT_STATUS=1" ) ;
    var = "SCRIPT_FILENAME=" + this->_path_ ;
    (*env)[size++] = strdup( var.c_str() ) ;
    (*env)[size++] = strdup("PHP_FCGI_MAX_REQUESTS=1");
    (*env)[size++] = strdup("PHP_INI_SCAN_DIR=/Users/klamqari/php/lib/php.ini");
    // var = "CONTENT_TYPE=";
    // (*env)[size++] = strdup(var.c_str());
    // var = "CONTENT_LENGTH=0";
    // (*env)[size++] = strdup(var.c_str());
    (*env)[size++] = NULL ;
}

void Response::execute_cgi( void )
{
    char **env;
    char *av[] = {
        (char *)"/Users/klamqari/php/bin/php-cgi",
        (char *)this->_path_.c_str(),
        NULL,
    };
    this->pid = fork() ;
    if (this->pid == -1)
    {
        this->status = 500;
        std::cout << "fork fail" << std::endl;
        // exit(4);
        // execute_cgi();
    }
    else if ( this->pid == 0 )
    {
        this->setup_environment(&env);
        if (close(this->s_fds[0])  == -1)
            exit(1) ;
        if (dup2(this->s_fds[1], 1) == -1 || dup2(this->s_fds[1], 0) == -1 )
            exit(1) ;
        if (close(this->s_fds[1])  == -1)
            exit(1) ;
        execve(av[0], av, env) ;
    }
}
