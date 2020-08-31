#include <stdio.h>
#include <string.h>
#include "ACCSockets.h"
#include "FileIO.h"

void getInput(char* arg1, char* arg2, char* arg3)
{
    //reset all the strings
    memset(arg1, 0, sizeof(char)*BUFFER_SIZE);
    memset(arg2, 0, sizeof(char)*BUFFER_SIZE);
    memset(arg3, 0, sizeof(char)*BUFFER_SIZE);

    char temp[BUFFER_SIZE];
    memset(temp, 0, sizeof(char)*BUFFER_SIZE);
    fgets(temp, BUFFER_SIZE, stdin);
    temp[strcspn(temp, "\n")] = 0;
    printf("%s\n", temp);

    int i = 0;
    int idx1 = 0, idx2 = 0, idx3 = 0;

    while ( i < strlen(temp) && temp[i++] != ' ')
    {
        arg1[idx1++]=temp[i-1];
    }
    while ( i < strlen(temp) && temp[i++] != ' ')
    {
        arg2[idx2++]=temp[i-1];
    }
    while ( i < strlen(temp) && temp[i++] != ' ')
    {
        arg3[idx3++]=temp[i-1];
    }

}

int storeFileClient(char* fileName, int sockfd)
{
    //Send the command to the server
    sendMessage("STORE", sockfd); 

    sendFile(fileName, sockfd);
 
    //Get the md5 key and display it
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*BUFFER_SIZE);
    recieveMessage(buffer, sockfd);

    printf("Key: %s\n", buffer);

    return 0;
}

int getFileClient(char* md5, char* fileName, int sockfd)
{
    sendMessage("GET", sockfd); 

    sendMessage(md5, sockfd); 

    char message[BUFFER_SIZE];
    memset(message, 0, sizeof(char)*BUFFER_SIZE);
    recieveMessage(message, sockfd);

    //If the file exists
    if (strncmp("exists",message,6) == 0)
    {
        recieveFile(fileName, sockfd);
        return 0;
    }
    else
    {
        printf("%s\n",message);
        return 1;
    }
}
