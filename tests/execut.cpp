#include <unistd.h>
#include <iostream>
#include <cstdlib>




int main()
{
    extern char **environ ;

    char *av[] = {
        (char *)"/usr/bin/php",
        (char *)"php.php",
        NULL
    };

    pid_t id = fork() ;

    if (id == 0)
    {
        setenv("REQUEST_METHOD", "GET", 1);
        execve(av[0], av, environ) ;
        std::cout << "execve fail" << std::endl ;
    }
    waitpid(id, NULL, 0) ;
}
