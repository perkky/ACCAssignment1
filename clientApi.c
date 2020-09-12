#include <stdio.h>
#include <string.h>
#include "ACCSockets.h"
#include "FileIO.h"
#include <signal.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

/* Function handler for SIGPIPE signal*/
void connectionLost(int signo)
{
    if (signo == SIGPIPE)
        printf("Error: Connection lost\n");

    exit(1);
}

/* Checks if the input is a valid IPv4 address 
 * Returns 0 if it is and 1 if it isnt.*/
int isValidIpAddress(char* ipAddr)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddr, &(sa.sin_addr));
    return result != 0;

}

/* Checks to see if the input string is a valid ip addres or a valid hostname.
 * Exports the output ip in outIp.
 * Returns 0 if it is able to get an ip and 1 if it isnt. */
int getIpAddr(char* inIpOrHost, char* outIp)
{
    if (isValidIpAddress(inIpOrHost))
    {
        strcpy(outIp, inIpOrHost);
    }
    else
    {
        struct hostent* host = gethostbyname(inIpOrHost);
        struct in_addr a;
        if (host != NULL)
        {
            bcopy(*host->h_addr_list++, (char*)&a, sizeof(a));
            strcpy(outIp, inet_ntoa(a));
        }
        else
            return 1;

    }

    return 0;
}

/* Gets the input from the user. Gets up to 3 inputs, each seperated by a space.
 * It returns the number of successful inputs read (0 to 3) */
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

/* The function sends the 'Quit' command to the server, waits for the response,
 * prints it to the user and returns. Returns SUCCESS if successfule 
 * or TIME_OUT if it times out.*/
int quitClient(int sockfd)
{
    //Send the command to the server
    sendMessage("QUIT", sockfd); 

    char message[BUFFER_SIZE];
    if (recieveMessage(message, sockfd) == TIME_OUT)
        return TIME_OUT;

    printf("%s\n", message);
    return SUCCESS;
} 

/* This function sends the 'store' command to the server, stores the file and prints the ket to the user.
 * If the file is not found, ERROR is returned. TIMEOUT is returned if it
 * times out and success is returned if it is successful.*/
int storeFileClient(char* fileName, int sockfd)
{
    if (!fileExistsClient(fileName))
    {
        printf("Error: File does not exist\n");
        return ERROR;
    }
    //Send the command to the server
    sendMessage("STORE", sockfd); 

    sendFile(fileName, sockfd);
 
    //Get the md5 key and display it
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    if (recieveMessage(buffer, sockfd) == TIME_OUT)
        return TIME_OUT;

    printf("%s\n", buffer);

    return SUCCESS;
}

/* This function sends the 'get' command to the server and retrieves the file.
 * If the file is not found, and error message is printed and INVALID_PARAMETER is returned. 
 * TIMEOUT is returned if it times out and success is returned if it is successful.*/
int getFileClient(char* md5, char* fileName, int sockfd)
{ 
    sendMessage("GET", sockfd); 

    sendMessage(md5, sockfd); 

    char message[BUFFER_SIZE];
    memset(message, 0, BUFFER_SIZE);
    if (recieveMessage(message, sockfd) == TIME_OUT)
        return TIME_OUT;

    //If the file exists
    if (strncmp("exists",message,6) == 0)
    {
        recieveFile(fileName, sockfd);
        return SUCCESS;
    }
    else
    {
        printf("%s\n",message);
        return INVALID_PARAMETER;
    }
}

/* This function sends the 'delete' command to the server and deletes the file.
 * If the file is not found, and error message is printed and INVALID_PARAMETER is returned. 
 * TIMEOUT is returned if it times out and success is returned if it is successful.*/
int deleteFileClient(char* md5, int sockfd)
{
    sendMessage("DELETE", sockfd); 

    sendMessage(md5, sockfd); 

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    if (recieveMessage(buffer, sockfd) == TIME_OUT)
        return TIME_OUT;

    printf("%s\n", buffer);

    return SUCCESS; 
}

/* This function sends the 'history' command to the server and retrieves the history of the file.
 * If the file is not found, and error message is printed and INVALID_PARAMETER is returned. 
 * TIMEOUT is returned if it times out and success is returned if it is successful.*/
int historyFileClient(char* md5, int sockfd)
{
    sendMessage("HISTORY", sockfd); 
    sendMessage(md5, sockfd); 

    char message[BUFFER_SIZE];
    memset(message, 0, BUFFER_SIZE);
    if (recieveMessage(message, sockfd) == TIME_OUT)
        return TIME_OUT;

    //If the file exists
    if (strncmp("exists",message,6) == 0)
    {
        memset(message, 0, BUFFER_SIZE);
        if (recieveMessage(message, sockfd) == TIME_OUT)
            return TIME_OUT;
        int size = atoi(message);

        for (int i = 0; i < size; i++)
        {
            memset(message, 0, BUFFER_SIZE);
            if (recieveMessage(message, sockfd) == TIME_OUT)
                return TIME_OUT; 

            printf("%s\n", message);
        }


        return SUCCESS;
    }
    else
    {
        printf("%s\n",message);
        return INVALID_PARAMETER;
    }
}
