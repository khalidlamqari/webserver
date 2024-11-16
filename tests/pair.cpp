
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main()
{
    int sv[2];

    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);

    int id = fork();
    if ( id == 0 )
    {
        const char s[] = "hello form child" ;
        write(sv[1], s, sizeof(s));
    }
    else
    {
        waitpid(id, NULL, 0) ;
        char buff[200];
        int i = read( sv[0], buff ,200 );
        buff[i] = '\0';
        printf("message : %s\n", buff);
    }

}
