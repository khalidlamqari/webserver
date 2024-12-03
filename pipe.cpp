#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int pipefd[2];  // pipefd[0] - read end, pipefd[1] - write end
    pid_t pid;
    char buffer[128];

    // Create the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork a new process
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {  // Parent process (reading from pipe)
        // Close the write end of the pipe as we are reading from the pipe
        close(pipefd[1]);

        // Read from the pipe
        ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';  // Null-terminate the string
            printf("Parent process received: %s\n", buffer);
        }

        // Close the read end after reading
        close(pipefd[0]);
    } else {  // Child process (writing to pipe)
        // Close the read end of the pipe as we are writing to the pipe
        close(pipefd[0]);

        // Write to the pipe
        const char *message = "Hello from the child process!";
        write(pipefd[1], message, sizeof(message));

        // Close the write end after writing
        close(pipefd[1]);
    }

    return 0;
}
