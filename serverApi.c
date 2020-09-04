#include <stdio.h>
#include "ACCSockets.h"
#include "serverApi.h"
#include <ctype.h>
#include <string.h>
#include "FileIO.h"
#include "history.h"
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include "global.h"
#include <sys/mman.h>

void sig_chld(int signo)
{
    int stat;
    if (signo == SIGCHLD)
    {
        wait(&stat);
    }

}

void createSharedMemory()
{
    g_history = (History*)mmap(NULL, sizeof(History), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED,-1,0);

    g_history->maxSize = MAX_NUM_HISTORY;
    g_history->size = 0;


    history_sem = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED,-1,0);

}


time_t getTime()
{
    return time(NULL);
}

char* getTimeString()
{
    time_t time = getTime();
    struct tm* timeinfo = localtime(&time);

    //get rid of the newline character
    char* time_str = asctime(timeinfo);
    time_str[strlen(time_str)-1] = '\0';

    return time_str;

}

command getCommandFromClient(int sockfd)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    //If 1 is returned, timeout has been reached
    if (recieveMessage(buffer, sockfd) == 1)
        return INVALID;

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

int quitServer(int sockfd)
{
    sendMessage("Thank you for using our anonymous storage", sockfd);

    return 0;
}

int storeFileServer(FileList* fileList, int sockfd)
{
    char fileName[BUFFER_SIZE];
    memset(fileName, 0, BUFFER_SIZE);
    getRandomFileName(fileName, 5);

    while (fileExistsName(fileList, fileName))
        getRandomFileName(fileName, 5);
    
    addStoragePrefixToFileName(fileName, 0);
    recieveFile(fileName, sockfd);

    char md5[BUFFER_SIZE];
    memset(md5, 0, BUFFER_SIZE);

    getMD5Sum(fileName, md5);

    sendMessage(md5, sockfd);
    
    freeFileList(fileList);
    getStorageFileList(fileList);


    sem_wait(history_sem);
    addHistoryLine(g_history, getTimeString(), md5, "STORE", "127.0.0.1"); 
    sem_post(history_sem);

    return 0;
}

int getFileServer(FileList* fileList, int sockfd)
{
    char md5[BUFFER_SIZE];
    memset(md5, 0, BUFFER_SIZE);
    recieveMessage(md5, sockfd);
    printf("md5: %s\n",md5);

    freeFileList(fileList);
    getStorageFileList(fileList);

    if (!fileExists(fileList, md5))
    {
        sendMessage("GET: Error! Hash key is not valid", sockfd); 
        return INVALID_PARAMETER;
    }
    else
    {
        sendMessage("exists", sockfd); 

        char fileName[BUFFER_SIZE];
        getFileName(fileList, md5, fileName);
        addStoragePrefixToFileName(fileName, 0);
        sendFile(fileName, sockfd);

        sem_wait(history_sem);
        addHistoryLine(g_history, getTimeString(), md5, "GET", "127.0.0.1"); 
        sem_post(history_sem);
    }

    return SUCCESS;
}

int deleteFileServer(FileList* fileList, int sockfd)
{
    char md5[BUFFER_SIZE];
    memset(md5, 0, BUFFER_SIZE);
    recieveMessage(md5, sockfd);

    freeFileList(fileList);
    getStorageFileList(fileList);

    if (!fileExists(fileList, md5))
    {
        sendMessage("DELETE: Error!\n", sockfd); 
        return INVALID_PARAMETER;
    }
    else
    {
        deleteFile(fileList, md5);
        sendMessage("DELETE: File deleted\n", sockfd); 

        sem_wait(history_sem);
        addHistoryLine(g_history, getTimeString(), md5, "DELETE", "127.0.0.1"); 
        sem_post(history_sem);

        return SUCCESS;
    }
}

int historyFileServer(int sockfd)
{
    char md5[BUFFER_SIZE];
    memset(md5, 0, BUFFER_SIZE);
    recieveMessage(md5, sockfd);

    FileHistory* fh = getFileHistory(g_history, md5);
    if ( fh == NULL)
    {
        sendMessage("GET: Error! Hash key is not valid", sockfd); 
        return INVALID_PARAMETER;
    }
    else
    {
        sendMessage("exists", sockfd); 

        sem_wait(history_sem);

        //send the size of the history
        char historySize[BUFFER_SIZE];
        memset(historySize,0,BUFFER_SIZE);
        sprintf(historySize, "%d", fh->size);
        sendMessage(historySize, sockfd);
        
        for (int i = 0; i < fh->size; i++)
            sendMessage(fh->history[i], sockfd);

        addHistoryLine(g_history, getTimeString(), md5, "HISTORY", "127.0.0.1"); 
        
        sem_post(history_sem);

        return SUCCESS;
    }

}
