/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExecuteCgi.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 19:31:45 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/06 18:20:35 by klamqari         ###   ########.fr       */
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
    int size = this->request.get_headers().size() + 10;
    
    *env = (char **)malloc(sizeof(char *) * size );
    size = 0;
    std::string var;
    std::map<std::string, std::string> headers = this->request.get_headers() ;
    std::map< std::string , std::string>::iterator it ;
    
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

    // var = "CONTENT_LENGTH=" + this->request->con;

    // (*env)[size++] = strdup(var.c_str());
    for (it = headers.begin(); it != headers.end(); ++it)
    {
        var = it->first + "=" + it->second;
        (*env)[size++] = strdup( var.c_str() ) ;
    }
    (*env)[size++] = NULL ;
}

std::string get_path(int fd)
{
    char buff[40];
    std::string path;
    ssize_t size = 1;
    
    while (size > 0)
    {   
        size = read(fd, buff, 39);
        if ( size > 0 )
        {
            buff[size] = '\0';
            path.append(buff, size);
        }
    }
    return path;
}

void Response::execute_cgi( void )
{
    char **env;
    std::string path;
    
    char *av[] = {
        (char *)"/Users/klamqari/php/bin/php-cgi",
        (char *)this->_path_.c_str(),
        NULL,
    };
    this->setup_environment(&env);
    std::cout << this->data_path << std::endl;

    this->pid = fork() ;
    if (this->pid == -1)
    {
        this->status = 500;
        std::cout << "fork fail" << std::endl;
    }
    else if ( this->pid == 0 )
    {
        if (close(this->s_fds[0])  == -1)
            exit(1) ;
        
        // path = get_path(this->s_fds[1]);
        
        int fd = open(this->data_path.c_str(), O_RDONLY | O_RDWR | O_TRUNC | O_CREAT, 0777);
        if (fd == -1)
            exit(1);

        if (dup2(fd, 1) == -1 || dup2(this->s_fds[1], 0) == -1)
            exit(1);

        if (close(fd)  == -1 || close(this->s_fds[1])  == -1)
            exit(1);

        execve(av[0], av, env) ;
    }
    // else
    // {
    //     int i = 0;
    //     while (env[i])
    //     {
    //         free(env[i]);
    //         i++;
    //     }
    //     free(env);
    // }
}
