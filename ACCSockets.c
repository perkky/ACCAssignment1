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
#include <sys/types.h>
#include <unistd.h>

struct timeval TIMEOUT;

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

//returns 0 if successful, 1 if unsucessful
int sendMessage(char* message, int sockfd)
{
    int bytesSent;
    int msgLen = strlen(message);
    char charToSend;
    int i = 0;

    while (msgLen > i)
    {
        charToSend = *(message + i++);
        if (charToSend == 0 || charToSend == '\n')
            break;
        else if ((bytesSent = send(sockfd, &charToSend, 1,0)) <= 0)
            return 1;
    }
    
    //Send 0 to indicate the message is complete
    char null = 0;
    send(sockfd, &null, 1,0);

    printf("Sent message: %s\n", message);

    return 0;
}

//returns 0 if successful, 1 if unsuccessful (eg timeout)
//If the socket is disconnected, a signal is sent and the program is exited
int recieveMessage(char* dest, int sockfd)
{
    char charRecieved;
    int bytesRecieved;
    int selectReturn;
    int i = 0;

    fd_set set;
    FD_ZERO(&set);
    FD_SET(sockfd, &set);
    TIMEOUT.tv_sec = 10;

    while( (selectReturn = select(sockfd+1, &set, NULL, NULL, &TIMEOUT)) == 1 && (bytesRecieved = recv(sockfd, &charRecieved, 1, 0)) > 0)
    {
        if (charRecieved == 0 || charRecieved == '\n')
        {
            *(dest + i++) = 0;
            break;

        }
        else
            *(dest + i++) = charRecieved;

    }

    printf("Recieved message: %s\n", dest);

    //Return 1 if there was an error
    return selectReturn == 1 ? SUCCESS : DISCONNECTED;
}

int sendFile(char* fileName, int sockfd)
{
    FILE* f = fopen(fileName, "r");
    int bytesRead;

    char charToSend;

    while (( bytesRead = fread(&charToSend, sizeof(char), 1, f)) > 0 )
    {
        if (send(sockfd, &charToSend, 1, 0) < 0 )
                return -1;
    }

    //Send 0 to indicate the message is complete
    char null = 0;
    send(sockfd, &null, 1, 0); 

    return fclose(f);
}

int recieveFile(char* fileName, int sockfd)
{
    FILE* f = fopen(fileName, "w");

    char charRecieved;
    int bytesRead;

    while((bytesRead = recv(sockfd, &charRecieved, 1, 0)) > 0)
    {
        if (charRecieved == 0)
            break;
        fwrite(&charRecieved, sizeof(char), 1, f);
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
    int forkId = 1;
    while (forkId != 0)
    {
        if ( (connfd = accept(listenfd, (SA *) &cliaddr, &len)) < 0 ) {
            fprintf(stderr, "accept failed\n");
            return -1;
        }
        if( (ptr = inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff))) == NULL) {
            fprintf(stderr, "inet_ntop error \n");
            return -1;
        }
        forkId = fork();
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
