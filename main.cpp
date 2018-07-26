#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>

#define PORT 8080

using namespace std;

int main () {
    
    if(fork()) {

        sleep(1);

        int server_fd, new_socket;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);
        
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        
        // Forcefully attaching socket to the port 8080
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );
        
        // Forcefully attaching socket to the port 8080
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 1) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
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
        sprintf(program, "python3 main.py %d", PORT);
        system(program);
    }
    
    return 0;
}
