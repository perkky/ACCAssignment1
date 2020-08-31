#pragma once
#include "FileIO.h"

typedef enum command
{
    STORE,
    GET,
    DELETE,
    HISTORY,
    QUIT,
    INVALID
} command;

command getCommandFromClient(int sockfd);
int storeFileServer(int sockfd);
int getFileServer(FileList* fileList, int sockfd);
