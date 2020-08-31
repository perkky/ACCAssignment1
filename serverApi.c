#include <stdio.h>
#include "ACCSockets.h"
#include "serverApi.h"
#include <ctype.h>
#include <string.h>
#include "FileIO.h"

command getCommandFromClient(int sockfd)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(char)*BUFFER_SIZE);

    recieveMessage(buffer, sockfd);
    toLower(buffer);

    if (strncmp("store", buffer, 5) == 0)
        return STORE;
    else if (strncmp("get", buffer, 3) == 0)
        return GET;
    else if (strncmp("delete", buffer, 6) == 0)
        return DELETE;
    else if (strncmp("history", buffer, 7) == 0)
        return HISTORY;
    else if (strncmp("quit", buffer, 4) == 0)
        return QUIT;
    else
        return INVALID;
}

int storeFileServer(int sockfd)
{
    recieveFile("storage/test", sockfd);

    char md5[BUFFER_SIZE];
    memset(md5, 0, sizeof(char)*BUFFER_SIZE);
    getMD5Sum("storage/test", md5);

    sendMessage(md5, sockfd);

    return 0;
}

int getFileServer(FileList* fileList, int sockfd)
{
    char md5[BUFFER_SIZE];
    memset(md5, 0, sizeof(char)*BUFFER_SIZE);
    recieveMessage(md5, sockfd);

    if (!fileExists(fileList, md5))
    {
        sendMessage("GET: Error! Hash key is not valid", sockfd); 
        return -1;
    }
    else
    {
        sendMessage("exists", sockfd); 

        char fileName[BUFFER_SIZE];
        getFileName(fileList, md5, fileName);
        addStoragePrefixToFileName(fileName, 0);
        sendFile(fileName, sockfd);
    }

    return 0;
}
