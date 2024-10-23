#include <iostream>
#include <cstring>       // For memset
#include <sys/types.h>   // For data types
#include <sys/socket.h>  // For socket functions
#include <netinet/in.h>  // For sockaddr_in
#include <unistd.h>      // For close
#include <arpa/inet.h>   // For htons and inet_ntoa
#include <poll.h>        // For poll function

#define PORT1 8080
#define PORT2 9090
#define MAX_CLIENTS 100

int createSocket(int port) {
    int server_fd;
    struct sockaddr_in address;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

int main() {
    int server_fd1, server_fd2, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create sockets for two ports
    server_fd1 = createSocket(PORT1);
    server_fd2 = createSocket(PORT2);

    // Set up the poll file descriptors
    struct pollfd fds[MAX_CLIENTS];
    int num_fds = 2; // We start with 2 server sockets

    memset(fds, 0, sizeof(fds));

    // Add the two server sockets to the pollfd structure
    fds[0].fd = server_fd1;
    fds[0].events = POLLIN;

    fds[1].fd = server_fd2;
    fds[1].events = POLLIN;

    std::cout << "Server is listening on ports " << PORT1 << " and " << PORT2 << std::endl;

    while (true) {
        // Wait for events on any of the sockets
        int poll_count = poll(fds, num_fds, -1);

        if (poll_count < 0) {
            perror("poll error");
            exit(EXIT_FAILURE);
        }

        // Check each file descriptor for incoming data
        for (int i = 0; i < num_fds; ++i) {
            if (fds[i].revents & POLLIN) {
                // If there is an incoming connection on a server socket
                if (fds[i].fd == server_fd1 || fds[i].fd == server_fd2) {
                    new_socket = accept(fds[i].fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    if (new_socket < 0) {
                        perror("accept failed");
                        exit(EXIT_FAILURE);
                    }

                    std::cout << "New connection from " << inet_ntoa(address.sin_addr)
                              << " on port " << (fds[i].fd == server_fd1 ? PORT1 : PORT2) << std::endl;

                    // Add new socket to the pollfd structure
                    fds[num_fds].fd = new_socket;
                    fds[num_fds].events = POLLIN;
                    num_fds++;

                    if (num_fds >= MAX_CLIENTS) {
                        std::cerr << "Max clients reached!" << std::endl;
                        close(new_socket);
                    }
                } else {
                    // Handle data from an already connected client
                    char buffer[1024] = {0};
                    int valread = read(fds[i].fd, buffer, sizeof(buffer));
                    if (valread <= 0) {
                        // If the client has disconnected
                        std::cout << "Client disconnected on fd " << fds[i].fd << std::endl;
                        close(fds[i].fd);
                        fds[i].fd = -1; // Mark it as closed
                    } else {
                        std::cout << "Received: " << buffer << std::endl;
                        // Echo the message back to the client
                        send(fds[i].fd, buffer, strlen(buffer), 0);
                    }
                }
            }
        }

        // Compact the fds array to remove closed connections
        for (int i = 0; i < num_fds; ++i) {
            if (fds[i].fd == -1) {
                for (int j = i; j < num_fds - 1; ++j) {
                    fds[j] = fds[j + 1];
                }
                num_fds--;
                i--;
            }
        }
    }

    // Clean up
    close(server_fd1);
    close(server_fd2);
    
    return 0;
}
