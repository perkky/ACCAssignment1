#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "ACCSockets.h"
#include "FileIO.h"
#include <unistd.h>
#include "serverApi.h"
#include "history.h"
#include <signal.h>
#include "global.h"
#include <sys/mman.h>
 
History* g_history = NULL;
IpList* g_ipList = NULL;
sem_t* history_sem = NULL;
sem_t* ipList_sem = NULL;
sem_t* file_sem = NULL;
int SOCKET_WAIT_TIME = 30;
int MAX_NUM_INVALID = 5;
int TIMEOUT_TIME = 10;

//The Server main function
int main(int argc, char* argv[])
{
    if (argc != 4)
    {
		printf("Usage: ./server <k> <t1> <t2>\n");
        return 1;
    }
    MAX_NUM_INVALID = atoi(argv[1]);
    TIMEOUT_TIME = atoi(argv[2]);
    SOCKET_WAIT_TIME = atoi(argv[3]);
    if (MAX_NUM_INVALID < 1 || TIMEOUT_TIME < 1 || SOCKET_WAIT_TIME < 1)
    {
        printf("Error: k >= 1, t1 >= 1, t2 >= 1\n");
        return 1;
    }

    signal(SIGCHLD, sig_chld);

    FileList fl;
    getStorageFileList(&fl);
    createSharedMemory();
    initialiseHistory(g_history, MAX_NUM_HISTORY);
    initialiseIpList(g_ipList, TIMEOUT_TIME);
    sem_init(history_sem, 1, 1);
    sem_init(file_sem, 1, 1);
    sem_init(ipList_sem, 1, 1);

    int id;
    char ip[BUFFER_SIZE];
    if ((id = listenSocket(PORT, ip)) < 0)
    {
        printf("Error connecting to socket\n");
        return 1;
    }

    sem_wait(ipList_sem);
    int isBanned = isIpBanned(g_ipList, ip, getTime());
    sem_post(ipList_sem);
    if (isBanned)
    {
        sendMessage("Error: Unable to connect as this ip is timed out", id);
        return 0;
    }
    else
    {
        sendMessage("connected", id);
    }

    int exit = 0;
    int numInvalid = 0;
    while (!exit)
    {
        sendMessage("18811931> ", id);

        //Get command
        switch (getCommandFromClient(id))
        {
            case STORE:
                exit = storeFileServer(&fl, ip, id);
                break;
            case GET:
                exit = getFileServer(&fl, ip, id);
                break;
            case DELETE:
                exit = deleteFileServer(&fl, ip, id);
                break;
            case HISTORY:
                exit = historyFileServer(&fl, ip, id);
                break;
            case QUIT:
                quitServer(id);
                exit = 1;
                break;
            case INVALID:
                //If invalid, timeout has been reached
                exit = TIME_OUT;
                break;
            default:
                break;
        }

        if (exit == INVALID_PARAMETER)
        {
            exit = 0;
            if (++numInvalid >= MAX_NUM_INVALID)
            {
                sem_wait(ipList_sem);
                addIp(g_ipList, ip, getTime());
                sem_post(ipList_sem);
                exit = 1;
                sendMessage("Client disconnected as too many invalid parameters have been sent", id);
            }
        }
        else if (exit == TIME_OUT)
        {
            //exit if it has timed out
            exit = 1;
        }
        else
        {
            numInvalid = 0;
        }
    }

    freeFileList(&fl);
    close(id);

    return 0;
}
