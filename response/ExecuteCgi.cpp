/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExecuteCgi.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 19:31:45 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/07 15:01:15 by klamqari         ###   ########.fr       */
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

void    Response::setup_environment(std::vector<std::string> & string_env)
{
    int size = this->request.get_headers().size() + 10;

    size = 0;
    std::string var;
    std::map<std::string, std::string> headers = this->request.get_headers() ;
    std::map< std::string , std::string>::iterator it ;
    
    string_env.push_back("REQUEST_METHOD=" + this->request.get_request_method());
    string_env.push_back("QUERY_STRING=") ;
    string_env.push_back("PATH_INFO=" + this->_path_info) ;
    string_env.push_back("SERVER_PROTOCOL=HTTP/1.1" ) ;
    string_env.push_back("REDIRECT_STATUS=1" ) ;
    string_env.push_back("SCRIPT_FILENAME=" + this->_path_) ;
    string_env.push_back("PHP_FCGI_MAX_REQUESTS=1");
    string_env.push_back("PHP_INI_SCAN_DIR=/Users/klamqari/php/lib/php.ini");

    // var = "CONTENT_LENGTH=" + this->request->con;
    // (*env)[size++] = strdup(var.c_str());

    for (it = headers.begin(); it != headers.end(); ++it)
    {
        string_env.push_back(it->first + "=" + it->second);
    }

}
void string_to_char(char **env, std::vector<std::string> & string_env)
{
    std::vector<std::string>::iterator it = string_env.begin();

    int i = 0;

    for (; it != string_env.end(); ++it)
    {
        (env)[i++] = (char *)(*it).c_str();
    }
    (env)[i] = NULL;
}

void Response::execute_cgi( void )
{
    std::vector<std::string> string_env;
    char **env;
    char *av[] = {
        (char *)"/Users/klamqari/php/bin/php-cgi",
        (char *)this->_path_.c_str(),
        NULL,
    };
    this->setup_environment(string_env);
    env = new char*[string_env.size() + 1];
    string_to_char(env, string_env);
    
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
        if (dup2(this->s_fds[1], 1) == -1 || dup2(this->s_fds[1], 0) == -1)
            exit(1);

        if ( close(this->s_fds[1])  == -1)
            exit(1);
        execve(av[0], av, env) ;
    }
    else
    {
        delete env;
    }
}
