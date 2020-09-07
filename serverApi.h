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

void createSharedMemory();
char* getTimeString();
time_t getTime();
void sig_chld(int signo);
command getCommandFromClient(int sockfd);
int quitServer(int sockfd);
int storeFileServer(FileList* fileList, char* ip, int sockfd);
int getFileServer(FileList* fileList, char* ip, int sockfd);
int deleteFileServer(FileList* fileList, char* ip, int sockfd);
int historyFileServer(FileList* fileLIst, char* ip, int sockfd);
