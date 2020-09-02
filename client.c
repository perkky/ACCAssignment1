#include <stdio.h>
#include <unistd.h>
#include "ACCSockets.h"
#include <stdlib.h>
#include "string.h"
#include "clientApi.h"
#include "FileIO.h"
#include <signal.h>

//Client main function
int main(int argc, char* argv[])
{
	if (argc != 3) {
		printf("usage: a.out <IPaddress> <port>");
		return 1;
	}
    signal(SIGPIPE, connectionLost);

    int sockfd;

    if ((sockfd = connectToSocket(argv[1], atoi(argv[2]))) == -1)
    {
        printf("Error connecting to socket\n");
        return 1;
    }

    int returnFlag = SUCCESS;
    char message[512];
    memset(message, 0, sizeof(message));
    while (returnFlag != DISCONNECTED)
    {
        if (returnFlag == SUCCESS)
            recieveMessage(message, sockfd);

        printf("%s ", message);

        /*char input[BUFFER_SIZE];*/
        /*memset(input, 0, sizeof(input));*/
        /*fgets(input, BUFFER_SIZE, stdin);*/
        char in1[256], in2[256], in3[256];
        int numArg = getInput(in1, in2, in3);
        toLower(in1);
        printf("%d\n", numArg);

        /*storeFileClient(in2, sockfd);*/
        if (strncmp("store", in1, 5) == 0 && numArg == 2)
            returnFlag = storeFileClient(in2, sockfd);
        else if (strncmp("get", in1, 3) == 0 && numArg == 3)
            returnFlag = getFileClient(in2, in3, sockfd);
        else if (strncmp("delete", in1, 6) == 0 && numArg == 2)
            returnFlag = deleteFileClient(in2, sockfd);
        else if (strncmp("history", in1, 7) == 0 && numArg == 2)
            returnFlag = historyFileClient(in2, sockfd);
        else if (strncmp("quit", in1, 4) == 0)
        {
            quitClient(sockfd);
            break;
        }
        else
        {
            printf("Error: Incorrect arguments\n");
            returnFlag = INVALID_PARAMETER;
        }
    }
    
    close(sockfd);

    return 0;
}
