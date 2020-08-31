#include <stdio.h>
#include <unistd.h>
#include "ACCSockets.h"
#include <stdlib.h>
#include "string.h"
#include "clientApi.h"
#include "FileIO.h"

//Client main function
int main(int argc, char* argv[])
{
	if (argc != 3) {
		printf("usage: a.out <IPaddress> <port>");
		return 1;
	}

    int sockfd;

    if ((sockfd = connectToSocket(argv[1], atoi(argv[2]))) == -1)
    {
        printf("Error connecting to socket\n");
        return 1;
    }

    char message[512];
    memset(message, 0, sizeof(message));
    recieveMessage(message, sockfd);
    printf("Message recieved\n");

    printf("%s ", message);

    /*char input[BUFFER_SIZE];*/
    /*memset(input, 0, sizeof(input));*/
    /*fgets(input, BUFFER_SIZE, stdin);*/
    char in1[256], in2[256], in3[256];
    getInput(in1, in2, in3);
    toLower(in1);
    printf("%s-%s-%s\n", in1, in2, in3);

    /*storeFileClient(in2, sockfd);*/
    if (strncmp("store", in1, 5) == 0)
        storeFileClient(in2, sockfd);
    else if (strncmp("get", in1, 3) == 0)
        getFileClient(in2, in3, sockfd);
    
    close(sockfd);

    return 0;
}
