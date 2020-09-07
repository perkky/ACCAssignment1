#pragma once
#define	SA struct sockaddr
#define BUFFER_SIZE 256
#include <netinet/in.h>

extern int SOCKET_WAIT_TIME;

enum ReturnVal { SUCCESS, DISCONNECTED, INVALID_PARAMETER, ERROR, TIME_OUT};

int listenSocket(int port, char* returnIp);

int sendFile(char* fileName, int sockfd);

int recieveFile(char* fileName, int sockfd);

int connectToSocket(char* ip_addr, int port);

int recieveMessage(char* dest, int sockfd);

int sendMessage(char* message, int sockfd);
