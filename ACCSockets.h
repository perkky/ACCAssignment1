#pragma once
#define	SA struct sockaddr
#define BUFFER_SIZE 256

enum ReturnVal { SUCCESS, DISCONNECTED, INVALID_PARAMETER, ERROR};

int listenSocket(int port);

int sendFile(char* fileName, int sockfd);

int recieveFile(char* fileName, int sockfd);

int connectToSocket(char* ip_addr, int port);

int recieveMessage(char* dest, int sockfd);

int sendMessage(char* message, int sockfd);
