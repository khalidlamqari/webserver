/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExecuteCgi.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 19:31:45 by klamqari          #+#    #+#             */
/*   Updated: 2024/12/27 10:18:31 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

/*
(char *)"REDIRECT_STATUS=1",
(char *)"SCRIPT_FILENAME=/path/to/your/script.php",
(char *)"QUERY_STRING=param1=value1&param2=value2",
*/

void    Response::setup_environment(std::vector<std::string> & string_env)
{
    std::string var;
    std::map<std::string, std::string> headers = this->clientsocket.get_request()->get_headers() ;
    std::map< std::string , std::string>::iterator it ;

    string_env.push_back("REQUEST_METHOD=" + this->clientsocket.get_request()->get_method());
    // string_env.push_back("CONTENT_LENGTH=11");
    string_env.push_back("CONTENT_TYPE=text/plain");
    string_env.push_back("REQUEST_METHOD=POST");
    string_env.push_back("QUERY_STRING=");
    string_env.push_back("PATH_INFO=" + this->_path_info);
    string_env.push_back("SERVER_PROTOCOL=HTTP/1.1" ) ;
    string_env.push_back("REDIRECT_STATUS=1" );
    string_env.push_back("SCRIPT_FILENAME=" + this->_path_);
    string_env.push_back("PHP_FCGI_MAX_REQUESTS=1");
    string_env.push_back("PHP_INI_SCAN_DIR=/Users/klamqari/php/lib/php.ini");

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
    char *av[3] = {
        (char *)"/Users/klamqari/php/bin/php-cgi",
        (char *)this->_path_.c_str(),
        NULL,
    };

    this->setup_environment(string_env);
    env = new char* [string_env.size() + 1];
    string_to_char(env, string_env);

    this->pid = fork() ;
    if (this->pid == -1)
        throw std::runtime_error("fork failed");


    if (this->pid == 0)
    {
        if (close(this->s_fds[0])  == -1)
            exit(1);

        int fd = open(this->input_path.c_str(), O_RDONLY , 0777);
        if (fd != -1 && (dup2(fd, 0) == -1 || close(fd) == -1))
        {
            exit(1);
        }

        if (dup2(this->s_fds[1], 1) == -1)
            exit(1);

        if (close(this->s_fds[1])  == -1)
            exit(1);

        execve(av[0], av, env);
        exit(1);
    }
    else
    {
        delete env;
    }
}
