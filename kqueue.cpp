#include <sys/event.h>
#include <sys/time.h>
#include <stdio.h>
# include <unistd.h>
#include <cstdlib>
#include <string>
#include <sys/socket.h>


void    setup_environment(char ***env)
{

    int size ;
    *env = (char **)malloc(sizeof(char *) * 20 );
    size = 0;
    std::string var;

    var = "REQUEST_METHOD=GET" ;
    (*env)[size++] = strdup( var.c_str() ) ;
    var = "QUERY_STRING=?" ;
    (*env)[size++] = strdup( var.c_str() ) ;
    var = "PATH_INFO=" ;
    (*env)[size++] = strdup( var.c_str() ) ;
    (*env)[size++] = strdup( "SERVER_PROTOCOL=HTTP/1.1" ) ;
    (*env)[size++] = strdup( "REDIRECT_STATUS=1" ) ;
    var = "SCRIPT_FILENAME=/Users/klamqari/Desktop/webserver/z_site//index.php";
    (*env)[size++] = strdup( var.c_str() ) ;
    (*env)[size++] = NULL ;
}

int  execute_cgi( void )
{
    char **env;
    char *av[] = {
        (char *)"/Users/klamqari/php/bin/php-cgi",
        (char *)"/Users/klamqari/Desktop/webserver/z_site//index.php",
        NULL,
    };
    int s_fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, s_fds);
    int pid = fork() ;
    if ( pid == 0 )
    {
        setup_environment(&env);
        if (close(s_fds[0])  == -1)
            exit(1) ;
        if (dup2(s_fds[1], 1) == -1)
            exit(1) ;
        execve(av[0], av, env) ;
        exit(1);
    }
    return pid;
}

void set_ev(int kq, int pid)
{
    struct kevent ev;
    EV_SET(&ev, pid, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT, 0, NULL);

    kevent(kq, &ev, 1, NULL, 0, NULL);
}

int main(int argc, char *argv[]) {

    int pid = execute_cgi();
    int kq = kqueue();

    set_ev(kq, pid);
    printf("Monitoring process %d for completion...\n", pid);

    struct kevent events[1000];
    while (1) {
        memset(events, 0, sizeof(events));
        int nevents = kevent(kq, NULL, 0, events, 1000, NULL);

        for (int i = 0; i < nevents; i++)
        {
            if (events[i].filter == EVFILT_PROC && (events[i].fflags & NOTE_EXIT))
            {
                printf("Process %d has exited.\n", (int)events[i].ident);
                close(kq);
                return 0;  
            }
        }
    }

    close(kq);
    return 0;
}






