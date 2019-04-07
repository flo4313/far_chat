#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define PORT 30200
#define CHAT_SIZE 2

struct client_pool{
    int csockfd[8];
    struct sockaddr_in csin[8]; 
    int cnum[8];
    int ccnum; // Number of connected client   
};

void createServer(int* socket_fd, struct sockaddr_in* sin);
void serverListen(int* socket_fd, int queue_size);
void initClientPool(struct client_pool*);
int acceptClient(int* sock_fd, struct sockaddr_in* csin);
int receiveStrMsg(int* csocket_fd, char * recv_buffer);
void sendStrMsg(int* socket_fd, char * send_buffer);


int main(int argc,char* argv[]){
    int sockfd; //server socket file descriptor
    struct sockaddr_in sin; //server address info;
    struct client_pool cpool; //client pool cointaining all the client connected to the server;
    
    initClientPool(&cpool);
    createServer(&sockfd, &sin);
    serverListen(&sockfd, CHAT_SIZE);
    
    char send_buffer[32];
    int send_size;
    int i = 0;
    while(cpool.ccnum < CHAT_SIZE){
        const int CURR_C = i % CHAT_SIZE; // Current client : the client we are waiting the message from;
        cpool.csockfd[CURR_C] = acceptClient(&sockfd, &(cpool.csin[CURR_C]));
        
        if( cpool.csockfd[CURR_C] == -1 ){
            perror("accept() failed");
        } else {
            cpool.cnum[CURR_C] = i+1;
            cpool.ccnum++;
            sprintf(send_buffer, "Bonjour client %d", cpool.cnum[CURR_C]);
            sendStrMsg(&(cpool.csockfd[CURR_C]), send_buffer);
            i++;
        }
    }

    char recv_buffer[32];
    int recv_size;
    i = 0;
    while(!sleep(1)){
        const int CURR_C = i % CHAT_SIZE; // Current client : the client we are waiting the message from;
        recv_size = receiveStrMsg(&(cpool.csockfd[CURR_C]), recv_buffer);
        if(recv_size == -1){
            perror("recv() failed");
        } else {
            printf("Message du client %d : %s", cpool.cnum[CURR_C], recv_buffer);
            i++;
            if(strcmp(recv_buffer, "-1") == 0 ){
                close(cpool.csockfd[CURR_C]);
            }
        }
    }
    return 0;
}

void initClientPool(struct client_pool* cpool) {
    cpool->ccnum = 0;
}

// Create the server with TCP protocol, accept any addresses
void createServer(int* socket_fd, struct sockaddr_in* sin){
    const int       optVal = 1;
    const socklen_t optLen = sizeof(optVal);
    *socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen);

    sin->sin_family = AF_INET;
    sin->sin_port = htons(PORT);
    sin->sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(*socket_fd, (struct sockaddr*) sin, sizeof(*sin)) == -1 ){
        perror("bind() failed");
        exit(errno);
    } else
    {
        printf("Server created on port %d \n", PORT);
    }
}

// Make the server listen on the defined port (defined in preprocess directives) with the defined queue size
void serverListen(int* socket_fd, int queue_size){
    if( listen(*socket_fd, queue_size) == -1 ) {
        perror("listen() failed");
        exit(errno);
        
    } else
    {
        printf("Server listening on port %d \n", PORT);
    }
}

// Send an int message to the destination
void sendIntMsg(int* socket_fd, int* send_buffer){
    if( send(*socket_fd, send_buffer, sizeof(*send_buffer), 0) == -1 ){
        perror("send() failed");
    }
}

// Send a string message to the to destination
void sendStrMsg(int* socket_fd, char * send_buffer){
    if( send(*socket_fd, send_buffer, strlen(send_buffer), 0) == -1 ){
        perror("send() failed");
    }
}

// Receive a string message from the client
int receiveStrMsg(int* csocket_fd, char * recv_buffer){
    char * recv_buffer_tmp = recv_buffer;
    int recv_size = recv(*csocket_fd, recv_buffer_tmp, sizeof(recv_buffer_tmp), 0);
    if(recv_size == -1){
        perror("recv() failed");
    } 
    return recv_size;
}

// Accept a TCP connection from a client, return the socket established with the client
int acceptClient(int* sock_fd, struct sockaddr_in* csin){
    char * cipaddr; 
    socklen_t csin_len = sizeof(*csin);
    int csocket_fd = accept(*sock_fd, (struct sockadrr * restrict)csin, &csin_len);
    if (csocket_fd == -1){
        perror("accept() failed");
    } else {
        cipaddr = inet_ntoa(csin->sin_addr);
        printf("Connection accepted with address %s \n", cipaddr);
    }
    

    return csocket_fd;
}