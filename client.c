#include <stdio.h>
#include <unistd.h>
#include "ACCSockets.h"
#include <stdlib.h>
#include "string.h"
#include "clientApi.h"
#include "FileIO.h"
#include <signal.h>

int SOCKET_WAIT_TIME = 3;

//Client main function
int main(int argc, char* argv[])
{
	if (argc != 3) {
		printf("Usage: ./client <IPaddress/hostname> <port>\n");
		return 1;
	}
    signal(SIGPIPE, connectionLost);

    char ip[BUFFER_SIZE];
    if (getIpAddr(argv[1], ip) != 0)
    {
        printf("Error: Unable to resolve hostname\n");
        return 1;
    }

    int sockfd;
    if ((sockfd = connectToSocket(ip, atoi(argv[2]))) == -1)
    {
        printf("Error connecting to socket\n");
        return 1;
    }

    int returnFlag = SUCCESS;
    char message[BUFFER_SIZE];
    memset(message, 0, sizeof(message));

    //Get the message from the server to make sure the client has been connected
    recieveMessage(message, sockfd);

    //If the server refuses the connection, print the message and quit
    if (strcmp(message, "connected") != 0)
    {
        printf("%s\n", message);
        return 0;
    }


    while (returnFlag != DISCONNECTED && returnFlag != TIME_OUT)
    {
        if (returnFlag == SUCCESS || returnFlag == INVALID_PARAMETER)
            recieveMessage(message, sockfd);

        printf("%s ", message);

        //If the message begins with "Client", it means the client has been rejected
        if (strncmp(message, "Client", 6) == 0)
        {
            printf("\n");
            return 0;
        }

        char in1[BUFFER_SIZE], in2[BUFFER_SIZE], in3[BUFFER_SIZE];
        int numArg = getInput(in1, in2, in3);
        toLower(in1);

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
            returnFlag = ERROR;
        }

        if (returnFlag == TIME_OUT)
        {
            printf("Error: Lost connection to host\n");
        }
    }
    
    close(sockfd);

    return 0;
}
