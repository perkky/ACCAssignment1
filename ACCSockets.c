#include <time.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include "ACCSockets.h"
#include <stdlib.h>

void sendStringLength(char* str, int sockfd)
{
    char str_len[BUFFER_SIZE];
    sprintf(str_len, "%d", (int)strlen(str));
    send(sockfd, str_len, BUFFER_SIZE,0);
}

void sendFileSize(FILE *f, int sockfd)
{
    int fileSize = 0;

    fseek(f, 0L, SEEK_END);
    fileSize = ftell(f);
    rewind(f);

    char file_size[BUFFER_SIZE];
    sprintf(file_size, "%d", fileSize);

    printf("File size: %d\n",fileSize);
    printf("File size: %s\n",file_size);

    send(sockfd, file_size, BUFFER_SIZE,0);
}

int sendMessage(char* message, int sockfd)
{
    int bytesToSend;
    int bytesSent;
    int msgLen = strlen(message);
    int i = 0;
    char buffer[BUFFER_SIZE];

    //send the length
    sendStringLength(message, sockfd);

    while (i*BUFFER_SIZE < msgLen)
    {
        bytesToSend = msgLen - i*BUFFER_SIZE;
        bytesToSend = bytesToSend >= BUFFER_SIZE ? BUFFER_SIZE : bytesToSend;
        strncpy(buffer, message+i*BUFFER_SIZE, bytesToSend);

        if ((bytesSent = send(sockfd, buffer, bytesToSend,0)) < bytesToSend)
            return -1;

        i++;
    }

    printf("Sent message: %s\n", message);

    return 0;
}

int recieveMessage(char* dest, int sockfd)
{
    char buffer[BUFFER_SIZE];
    int bytesRead;

    int sizeOfMessage = 0;
    char str_sizeOfMessage[BUFFER_SIZE];
    recv(sockfd, str_sizeOfMessage, BUFFER_SIZE, 0);
    sizeOfMessage = atoi(str_sizeOfMessage);

    while(sizeOfMessage > 0 && (bytesRead =recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0)
    {
        strncat(dest, buffer, bytesRead);
        sizeOfMessage -= bytesRead;
    }

    printf("Recieved message: %s\n", dest);

    return 0;
}

int sendFile(char* fileName, int sockfd)
{
    FILE* f = fopen(fileName, "r");
    //Send file size?
    char buffer[BUFFER_SIZE];
    int bytesRead;
    int bytesSent;

    sendFileSize(f, sockfd);

    while (( bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, f)) > 0 )
    {
        if ( (bytesSent = send(sockfd, buffer, bytesRead, 0)) < bytesRead )
                return -1;
        printf("Sent line: %d\n", bytesRead);
    }

    return fclose(f);
}

int recieveFile(char* fileName, int sockfd)
{
    FILE* f = fopen(fileName, "w");

    char buffer[BUFFER_SIZE];
    int bytesRead;

    int sizeOfMessage = 0;
    char str_sizeOfMessage[BUFFER_SIZE];
    recv(sockfd, str_sizeOfMessage, BUFFER_SIZE, 0);
    sizeOfMessage = atoi(str_sizeOfMessage);
    printf("File size: %d\n", sizeOfMessage);

    while(sizeOfMessage > 0 && (bytesRead =recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0)
    {
        fwrite(buffer, sizeof(char), bytesRead, f);
        sizeOfMessage -= bytesRead;
        printf("Size left: %d\n", sizeOfMessage);
    }

    return fclose(f);
}

int listenSocket(int port)
{
	int			listenfd, connfd;
	socklen_t		len;
	struct sockaddr_in	servaddr, cliaddr;
	char			buff[BUFFER_SIZE];
	const char	*ptr;

	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		fprintf(stderr, "socket creation failed\n");
        return -1;
	}
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    {
        fprintf(stderr, "setsockopt failed\n");
        return -1;
    }

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port); /* daytime server */

	if ( (bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0) {
		fprintf(stderr, "bind failed\n");
        return -1;
	}
		

	if ( listen(listenfd, 1024) < 0) {
		fprintf(stderr, "listen failed\n");
        return -1;
	}
    len = sizeof(cliaddr);
    if ( (connfd = accept(listenfd, (SA *) &cliaddr, &len)) < 0 ) {
        fprintf(stderr, "accept failed\n");
        return -1;
    }
    if( (ptr = inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff))) == NULL) {
        fprintf(stderr, "inet_ntop error \n");
        return -1;
    }

    return connfd;
}

int connectToSocket(char* ip_addr, int port)
{
	int	sockfd;
	struct sockaddr_in	servaddr;


	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("socket error");
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(port);	/* daytime server */
	if (inet_pton(AF_INET, ip_addr, &servaddr.sin_addr) <= 0){
		printf("inet_pton error for %s", ip_addr);
		return -1;
	}

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
		printf("connect error");
		return -1; 
	}

    return sockfd;
}
