#include <stdio.h>
#include <unistd.h>
#include "ACCSockets.h"
#include <stdlib.h>
#include "string.h"
#include "clientApi.h"
#include "FileIO.h"
#include <signal.h>
#include <netdb.h>

int SOCKET_WAIT_TIME = 3;

//Client main function
int main(int argc, char* argv[])
{
	if (argc != 3) {
		printf("Usage: ./client <IPaddress> <port>\n");
		return 1;
	}
    signal(SIGPIPE, connectionLost);

    struct hostent* host = gethostbyname(argv[1]);
    char str[512];
    inet_ntop(AF_INET, &host->h_addr_list[0], str, INET_ADDRSTRLEN);
    printf("Ip address = %s\n", str);



    int sockfd;
    if ((sockfd = connectToSocket(argv[1], atoi(argv[2]))) == -1)
    {
        printf("Error connecting to socket\n");
        return 1;
    }

    int returnFlag = SUCCESS;
    char message[512];
    memset(message, 0, sizeof(message));

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

        if (strncmp(message, "Client", 6) == 0)
        {
            printf("\n");
            return 0;
        }

        /*char input[BUFFER_SIZE];*/
        /*memset(input, 0, sizeof(input));*/
        /*fgets(input, BUFFER_SIZE, stdin);*/
        char in1[256], in2[256], in3[256];
        int numArg = getInput(in1, in2, in3);
        toLower(in1);

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
            returnFlag = ERROR;
        }

        if (returnFlag == TIME_OUT)
            printf("Error: Lost connection to host\n");
    }
    
    close(sockfd);

    return 0;
}
