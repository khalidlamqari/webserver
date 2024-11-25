#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program> [args...]\n", argv[0]);
        return 1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        char *program = argv[1];
        char **args = &argv[1];
        
        if (execve(program, args, NULL) == -1) {
            perror("execve");
            exit(1);
        }
    }

    int kq = kqueue();
    if (kq == -1) {
        perror("kqueue");
        return 1;
    }
    
    struct kevent ev;
    EV_SET(&ev, pid, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT, 0, NULL);

    if (kevent(kq, &ev, 1, NULL, 0, NULL) == -1) {
        perror("kevent");
        close(kq);
        return 1;
    }

    printf("Monitoring process %d for completion...\n", pid);
    struct kevent events[1];
    int nevents = kevent(kq, NULL, 0, events, 1, NULL);
    if (nevents == -1) {
        perror("kevent");
        close(kq);
        return 1;
    }
    for (int i = 0; i < nevents; i++) {
        if (events[i].filter == EVFILT_PROC && events[i].fflags & NOTE_EXIT) {
            printf("Process %d has exited.\n", (int)events[i].ident);
        }
    }
    close(kq);
    return 0;
}
