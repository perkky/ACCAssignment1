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
int quitServer(int sockfd);
int storeFileServer(FileList* fileList, int sockfd);
int getFileServer(FileList* fileList, int sockfd);
int deleteFileServer(FileList* fileList, int sockfd);
