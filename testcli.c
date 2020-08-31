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
    while (( bytesRead = fread(buffer, BUFFER_SIZE, 1, f)) != EOF )
    {
        if ( send(sockfd, buffer, bytesRead, 0) < bytesRead )
                return -1;
    }

    return fclose(f);
}

int recieveFile(char* fileName, int sockfd)
{
    FILE* f = fopen(fileName, "w");

    char buffer[BUFFER_SIZE];
    int bytesRead;

    while( (bytesRead =recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0)
    {
        fwrite(buffer, sizeof(char), bytesRead, f);
        printf("recieve line: %d \n", bytesRead);
    }

    return fclose(f);
}

int main(int argc, char **argv)
{
	int	sockfd, n;
	struct sockaddr_in	servaddr;


	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("socket error");
		return 1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(53000);	/* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
		printf("inet_pton error for %s", argv[1]);
		return 1;
	}

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
		printf("connect error");
		return 1; 
	}

    recieveFile("storage/d", sockfd);
    
    return 0;
}
