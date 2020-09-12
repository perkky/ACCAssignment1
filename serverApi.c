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
#include <stdlib.h>

void sig_chld(int signo)
{
    int stat;
    if (signo == SIGCHLD)
    {
        wait(&stat);
    }

}

//Create the shared memory between each process
void createSharedMemory()
{
    g_history = (History*)mmap(NULL, sizeof(History), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED,-1,0);

    g_history->maxSize = MAX_NUM_HISTORY;
    g_history->size = 0;
    g_ipList = (IpList*)mmap(NULL, sizeof(IpList), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED,-1,0);




    history_sem = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED,-1,0);
    ipList_sem = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED,-1,0);

}

void destroySharedMemory()
{
    munmap(g_history, sizeof(History*));
    munmap(g_ipList, sizeof(IpList*));
    munmap(history_sem, sizeof(sem_t)); 
    munmap(ipList_sem, sizeof(sem_t)); 
}


time_t getTime()
{
    return time(NULL);
}

//This function returns the current time as a string
char* getTimeString()
{
    time_t time = getTime();
    struct tm* timeinfo = localtime(&time);

    //get rid of the newline character
    char* time_str = asctime(timeinfo);
    time_str[strlen(time_str)-1] = '\0';

    return time_str;

}

/* This function waits for a message from the client and returns the command.
 */
command getCommandFromClient(int sockfd)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    //If 1 is returned, timeout has been reached
    if (recieveMessage(buffer, sockfd) == TIME_OUT)
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

//The function to be run when the client requests 'Quit'
int quitServer(int sockfd)
{
    sendMessage("Thank you for using our anonymous storage", sockfd);

    return SUCCESS;
}


/* This function is run when the client requests to store a file.
 * It saves the sent file as a random file name and adds it to
 * the history and SUCCESS is returned. */
int storeFileServer(FileList* fileList, char* ip, int sockfd)
{
    char fileName[BUFFER_SIZE];
    memset(fileName, 0, BUFFER_SIZE);
    getRandomFileName(fileName, 5);
    freeFileList(fileList);
    getStorageFileList(fileList);

    while (fileExistsName(fileList, fileName))
        getRandomFileName(fileName, 5);
    
    addStoragePrefixToFileName(fileName, 0);
    recieveFile(fileName, sockfd);

    char md5[BUFFER_SIZE];
    memset(md5, 0, BUFFER_SIZE);

    getMD5Sum(fileName, md5);

    //If file already exists, delete it
    if (fileExists(fileList, md5))
    {
        deleteFile(fileList, md5);
    }

    char dest[2*BUFFER_SIZE];
    sprintf(dest,"STORE: File has been stored with hash %s", md5);

    sendMessage(dest, sockfd);
    
    freeFileList(fileList);
    getStorageFileList(fileList);


    sem_wait(history_sem);
    addHistoryLine(g_history, getTimeString(), md5, "STORE", ip); 
    sem_post(history_sem);

    return SUCCESS;
}

/* This function is run when the client requests to get a file.
 * If the file is not found, it returns an error message to the client
 * and INVALID_PARAMETER is returned.
 * If the files is found, it sends the file to the client and SUCCESS
 * is returned.*/
int getFileServer(FileList* fileList, char* ip, int sockfd)
{
    char md5[BUFFER_SIZE];
    memset(md5, 0, BUFFER_SIZE);
    if (recieveMessage(md5, sockfd) == TIME_OUT)
        return INVALID;

    freeFileList(fileList);
    getStorageFileList(fileList);

    if (!fileExists(fileList, md5))
    {
        char dest[2*BUFFER_SIZE];
        sprintf(dest,"GET: Error! Hash %s is not valid", md5);
        sendMessage(dest, sockfd); 
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
        addHistoryLine(g_history, getTimeString(), md5, "GET", ip); 
        sem_post(history_sem);
    }

    return SUCCESS;
}

/* This function is run when the client requests to delete a file.
 * If the file is not found, an error message is sent to the client
 * and INVALID_PARAMETER is returned.
 * If the file is found, it is deleted and SUCCESS is returned.*/
int deleteFileServer(FileList* fileList, char* ip, int sockfd)
{
    char md5[BUFFER_SIZE];
    memset(md5, 0, BUFFER_SIZE);
    if (recieveMessage(md5, sockfd) == TIME_OUT)
        return INVALID;

    freeFileList(fileList);
    getStorageFileList(fileList);

    if (!fileExists(fileList, md5))
    {
        char dest[2*BUFFER_SIZE];
        sprintf(dest,"DELETE: Error! Hash %s is not valid", md5);
        sendMessage(dest, sockfd); 
        return INVALID_PARAMETER;
    }
    else
    {
        deleteFile(fileList, md5);
        char dest[2*BUFFER_SIZE];
        sprintf(dest,"DELETE: File with hash %s has been deleted", md5);
        sendMessage(dest, sockfd); 

        sem_wait(history_sem);
        addHistoryLine(g_history, getTimeString(), md5, "DELETE", ip); 
        sem_post(history_sem);

        return SUCCESS;
    }
}

/* This function is run when the client requests to get the history of a file.
 * If the file is not found, an error message is sent to the client
 * and INVALID_PARAMETER is returned.
 * If the file is found, it is the history of the file is sent and SUCCESS is returned.*/
int historyFileServer(FileList* fileList, char* ip, int sockfd)
{
    char md5[BUFFER_SIZE];
    memset(md5, 0, BUFFER_SIZE);
    if (recieveMessage(md5, sockfd) == TIME_OUT)
        return INVALID;

    freeFileList(fileList);
    getStorageFileList(fileList);

    FileHistory* fh = getFileHistory(g_history, md5);
    if ( fh == NULL)
    {
        char dest[2*BUFFER_SIZE];
        sprintf(dest,"HISTORY: Error! There is not history for hash %s", md5);
        sendMessage(dest, sockfd); 

        if (fileExists(fileList, md5))
            addHistoryLine(g_history, getTimeString(), md5, "HISTORY", ip); 

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

        addHistoryLine(g_history, getTimeString(), md5, "HISTORY", ip); 
        
        sem_post(history_sem);

        return SUCCESS;
    }

}
