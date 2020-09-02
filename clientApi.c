#include <stdio.h>
#include <string.h>
#include "ACCSockets.h"
#include "FileIO.h"
#include <signal.h>
#include <stdlib.h>

void connectionLost(int signo)
{
    if (signo == SIGPIPE)
        printf("Error: Connection lost\n");

    exit(1);
}

int getInput(char* arg1, char* arg2, char* arg3)
{
    int argc1 =0, argc2 = 0, argc3 =0;
    //reset all the strings
    memset(arg1, 0, BUFFER_SIZE);
    memset(arg2, 0, BUFFER_SIZE);
    memset(arg3, 0, BUFFER_SIZE);

    char temp[BUFFER_SIZE];
    memset(temp, 0, BUFFER_SIZE);
    fgets(temp, BUFFER_SIZE, stdin);
    temp[strcspn(temp, "\n")] = 0;
    printf("%s\n", temp);

    int i = 0;
    int idx1 = 0, idx2 = 0, idx3 = 0;

    while ( i < strlen(temp) && temp[i++] != ' ')
    {
        arg1[idx1++]=temp[i-1];
        argc1 = 1;
    }
    while ( i < strlen(temp) && temp[i++] != ' ')
    {
        arg2[idx2++]=temp[i-1];
        argc2 = 1;
    }
    while ( i < strlen(temp) && temp[i++] != ' ')
    {
        arg3[idx3++]=temp[i-1];
        argc3 = 1;
    }

    return argc1 + argc2 + argc3;

}

int quitClient(int sockfd)
{
    //Send the command to the server
    sendMessage("QUIT", sockfd); 

    char message[BUFFER_SIZE];
    if (recieveMessage(message, sockfd) == DISCONNECTED)
        return DISCONNECTED;

    printf("%s\n", message);
    return SUCCESS;
} 

int storeFileClient(char* fileName, int sockfd)
{
    if (!fileExistsClient(fileName))
    {
        printf("Error: File does not exist\n");
        return INVALID_PARAMETER;
    }
    //Send the command to the server
    sendMessage("STORE", sockfd); 

    sendFile(fileName, sockfd);
 
    //Get the md5 key and display it
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    if (recieveMessage(buffer, sockfd) == DISCONNECTED)
        return DISCONNECTED;

    printf("Key: %s\n", buffer);

    return SUCCESS;
}

int getFileClient(char* md5, char* fileName, int sockfd)
{ 
    sendMessage("GET", sockfd); 

    sendMessage(md5, sockfd); 
    printf("md5: %s\n",md5);

    char message[BUFFER_SIZE];
    memset(message, 0, BUFFER_SIZE);
    if (recieveMessage(message, sockfd) == DISCONNECTED)
        return DISCONNECTED;
    printf("%s\n",message);

    //If the file exists
    if (strncmp("exists",message,6) == 0)
    {
        recieveFile(fileName, sockfd);
        return SUCCESS;
    }
    else
    {
        printf("%s\n",message);
        return SUCCESS;
    }
}

int deleteFileClient(char* md5, int sockfd)
{
    sendMessage("DELETE", sockfd); 

    sendMessage(md5, sockfd); 

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    if (recieveMessage(buffer, sockfd) == DISCONNECTED)
        return DISCONNECTED;

    printf("%s\n", buffer);

    return SUCCESS; 
}

int historyFileClient(char* md5, int sockfd)
{
    sendMessage("HISTORY", sockfd); 
    sendMessage(md5, sockfd); 

    char message[BUFFER_SIZE];
    memset(message, 0, BUFFER_SIZE);
    if (recieveMessage(message, sockfd) == DISCONNECTED)
        return DISCONNECTED;

    //If the file exists
    if (strncmp("exists",message,6) == 0)
    {
        memset(message, 0, BUFFER_SIZE);
        if (recieveMessage(message, sockfd) == DISCONNECTED)
            return DISCONNECTED;
        int size = atoi(message);

        for (int i = 0; i < size; i++)
        {
            memset(message, 0, BUFFER_SIZE);
            if (recieveMessage(message, sockfd) == DISCONNECTED)
                return DISCONNECTED; 

            printf("%s\n", message);
        }


        return SUCCESS;
    }
    else
    {
        printf("%s\n",message);
        return SUCCESS;
    }
}
