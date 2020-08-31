#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
const int BUFFER_SIZE = 256;
#define	SA struct sockaddr

int sendFile(char* fileName, int sockfd)
{
    FILE* f = fopen(fileName, "r");
    //Send file size?
    char buffer[BUFFER_SIZE];
    int bytesRead;
    int bytesSent;
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

    while( recv(sockfd, buffer, BUFFER_SIZE, 0) > 0)
    {
        fwrite(buffer, sizeof(char), BUFFER_SIZE, f);
    }

    return fclose(f);
}

int main(int argc, char **argv)
{
	int			listenfd, connfd;
	socklen_t		len;
	struct sockaddr_in	servaddr, cliaddr;
	char			buff[BUFFER_SIZE];
	const char	*ptr;

	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		fprintf(stderr, "socket creation failed\n");
		exit (1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(53000); /* daytime server */

	if ( (bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0) {
		fprintf(stderr, "bind failed\n");
		exit (1);
	}
		

	if ( listen(listenfd, 1024) < 0) {
		fprintf(stderr, "listen failed\n");
		exit (1);
	}

    for ( ; ; ) {
        len = sizeof(cliaddr);
        if ( (connfd = accept(listenfd, (SA *) &cliaddr, &len)) < 0 ) {
            fprintf(stderr, "accept failed\n");
            exit (1);
        }
    if( (ptr = inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff))) == NULL) {
        fprintf(stderr, "inet_ntop error \n");
        exit (1);
    }
        printf("Server: connection from client %s, port %d\n", ptr, ntohs(cliaddr.sin_port));
        sendFile("storage/a", connfd);

		if ( close(connfd) == -1) {
			fprintf(stderr, "accept failed\n");
			exit (1);
        }
    }
    
    return 0;
}
