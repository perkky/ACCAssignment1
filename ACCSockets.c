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

int getFileSize(char* fileName)
{
    int size = 0;
    FILE* fp = fopen(fileName, "r");
    fseek(fp, 0L, SEEK_END);

    size = (int)ftell(fp);

    return size;
}


/* Sends a message to socket with sockfd
 * This sends the whole message, only stopping at a new line character or 0.
 * Once the message has been sent, 0 is sent to indicate it is finished.
 * Returns SUCCESS if successful and ERROR if an error occurs*/
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
            return ERROR;
    }
    
    //Send 0 to indicate the message is complete
    char null = 0;
    send(sockfd, &null, 1,0);


    return SUCCESS;
}

/* Recieves a message from the socket socfd.
 * This continues to read characters from the socket until it reaches a 0,
 * as this indicates the end of the message has been reached.
 * select() is used to timout this if no respone is given in SOCKET_WAIT_TIME.
 * Returns SUCCESS if successful and TIME_OUT if it timesout. */
int recieveMessage(char* dest, int sockfd)
{
    char charRecieved;
    int bytesRecieved;
    int selectReturn;
    int i = 0;

    fd_set set;
    FD_ZERO(&set);
    FD_SET(sockfd, &set);
    TIMEOUT.tv_sec = SOCKET_WAIT_TIME;

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

    //Return 1 if there was an error
    return selectReturn == 1 ? SUCCESS : TIME_OUT;
}

/* Functions the same as sendMessage(), however it sends a file instead. */
int sendFile(char* fileName, int sockfd)
{
    int fileSize = getFileSize(fileName);
    //send the file size
    char fileSizeStr[BUFFER_SIZE];
    sprintf(fileSizeStr, "%d", fileSize);
    sendMessage(fileSizeStr, sockfd);
    
    FILE* f = fopen(fileName, "r");
    int bytesRead;

    char charToSend;

    while (( bytesRead = fread(&charToSend, sizeof(char), 1, f)) > 0 )
    {
        if (send(sockfd, &charToSend, 1, 0) < 0 )
                return -1;
    }


    return fclose(f);
}

/* Functions the same as recieveMessage(), however it recieves a file instead. */
int recieveFile(char* fileName, int sockfd)
{
    FILE* f = fopen(fileName, "w");

    //recieve file size
    char fileSizeStr[BUFFER_SIZE];
    recieveMessage(fileSizeStr, sockfd);
    int fileSize = atoi(fileSizeStr);

    char charRecieved;
    int bytesRead;

    while(fileSize-- > 0 && (bytesRead = recv(sockfd, &charRecieved, 1, 0)) > 0)
    {
        fwrite(&charRecieved, sizeof(char), 1, f);
    }

    return fclose(f);
}

/* Listens on a port. If a connection is created, fork() is called
 * and one process' function returns whilst the other continues to listen. */
int listenSocket(int port, char* returnIp)
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

    strcpy(returnIp, buff);

    return connfd;
}

/* Connects to a socket */
int connectToSocket(char* ip_addr, int port)
{
	int	sockfd;
	struct sockaddr_in	servaddr;


	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Socket error\n");
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(port);	/* daytime server */
	if (inet_pton(AF_INET, ip_addr, &servaddr.sin_addr) <= 0){
		printf("Inet_pton error for %s\n", ip_addr);
		return -1;
	}

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
		printf("Connection error\n");
		return -1; 
	}

    return sockfd;
}
