#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fds[2];

    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    write(fds[0], "1234", 4);
    std::string body = "helllllo";
    int f  = fork();
    if (f == 0)
    {
        // char buff[200];
        // int n = read(fds[1], buff, 200);
        // write(fd, buff, n);
        // int fd = open("_data_", O_RDONLY, 0777);
        // dup2(fd, 0);
        // close(fd);
        write(0, body.c_str(), body.length());
        char *av[3] = {
            (char *)"/Users/klamqari/php/bin/php-cgi",
            (char *)"./z_site/index.php",
            NULL,
        };

        execve(av[0], av, NULL);

    }
    else
    {
        
    }
}