#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <vector>
#include <utility>

int main () {

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = 0;
    
    // Creating socket file descriptor
    if (!(server_fd = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("error creating socket");
        exit(EXIT_FAILURE);
    }
    
    // Bind socket to random port
    if (bind(server_fd, (struct sockaddr *) &address, addrlen)) {
        perror("error binding to random port");
        exit(EXIT_FAILURE);
    }

    // Save randomly selected port
    if (getsockname(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) {
        perror("error saving random port");
        exit(EXIT_FAILURE);
    }
    int port = ntohs(address.sin_port);
    
    if(fork()) {

        // Listen and accept on socket
        if (listen(server_fd, 1) < 0) {
            perror("error listening to port");
            exit(EXIT_FAILURE);
        }  
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
            perror("error accepting connection");
            exit(EXIT_FAILURE);
        }

        // Get number of pairs
        unsigned int numPairs;
        read(new_socket, &numPairs, 4);
        numPairs = ntohl(numPairs);
        std::vector< std::pair<int,int> > path;

        // Fill path vector
        for (unsigned int i = 0; i < numPairs; i++) {
            std::pair<int,int> tuple;
            read(new_socket, &tuple.first, 4);
            read(new_socket, &tuple.second, 4);
            tuple.first = ntohl(tuple.first);
            tuple.second = ntohl(tuple.second);
            printf("index %d: (%d, %d)\n", i, tuple.first, tuple.second);
        }

        wait(NULL);

    // Run python script
    } else {
        char mainPyPath[PATH_MAX];
        sprintf(mainPyPath, "%s/python/cpp-io.py", getenv("PRACSYS_PATH"));
        char portStr[10];
        sprintf(portStr, "%d", port);
        execl("/usr/bin/python3", "python3", mainPyPath, portStr, NULL);
    }
    
    return 0;
}
