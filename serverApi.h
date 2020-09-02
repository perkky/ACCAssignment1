#pragma once
#include "FileIO.h"
#include "history.h"
#include <time.h>

typedef enum command
{
    STORE,
    GET,
    DELETE,
    HISTORY,
    QUIT,
    INVALID
} command;

extern FileHistory* g_startHistory;

char* getTimeString();
time_t getTime();
void sig_chld(int signo);
command getCommandFromClient(int sockfd);
int quitServer(int sockfd);
int storeFileServer(FileList* fileList, int sockfd);
int getFileServer(FileList* fileList, int sockfd);
int deleteFileServer(FileList* fileList, int sockfd);
int historyFileServer(int sockfd);
