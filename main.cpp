#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>

using namespace std;

int main () {

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = 0;
    
    // Creating socket file descriptor
    if (!(server_fd = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Bind socket to random port
    if (bind(server_fd, (struct sockaddr *) &address, addrlen)) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Save randomly selected port
    if (getsockname(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) {
        perror("error saving port");
        exit(EXIT_FAILURE);
    }
    int port = ntohs(address.sin_port);
    
    if(fork()) {

        if (listen(server_fd, 1) < 0) {
            perror("listen error");
            exit(EXIT_FAILURE);
        }
        
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
            perror("accept error");
            exit(EXIT_FAILURE);
        }

        unsigned int size;
        read(new_socket, &size, 4);
        size = ntohl(size);
        printf("size %d\n", size);
        send(new_socket, "hello back", 10, 0);
        printf("Hello message sent\n");

        wait(NULL);

    } else {
        char program[30];
        sprintf(program, "python3 main.py %d", port);
        system(program);
    }
    
    return 0;
}
