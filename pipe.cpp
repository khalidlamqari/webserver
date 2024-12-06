#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[128];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        close(pipefd[0]);
        const char *message = "Hello from the child process!";
        write(pipefd[1], message, sizeof(message));
        close(pipefd[1]);
    }
    else
    {
        wait(NULL);
        close(pipefd[1]);
        ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';  // Null-terminate the string
            printf("Parent process received: %s\n", buffer);
        }
        close(pipefd[0]);
    }
    return 0;
}
