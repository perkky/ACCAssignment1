#pragma once
#define	SA struct sockaddr
#define BUFFER_SIZE 256

int listenSocket(int port);

int sendFile(char* fileName, int sockfd);

int recieveFile(char* fileName, int sockfd);

int connectToSocket(char* ip_addr, int port);

int recieveMessage(char* dest, int sockfd);

int sendMessage(char* message, int sockfd);
