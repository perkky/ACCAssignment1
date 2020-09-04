#include <stdio.h>
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
sem_t* history_sem = NULL;

//The Server main function
int main(int argc, char* argv[])
{
    signal(SIGCHLD, sig_chld);

    FileList fl;
    getStorageFileList(&fl);
    for (int i = 0; i < fl.size; i++)
        printf("%s %s\n",fl.fileList[i], fl.md5List[i]);

    createSharedMemory();
    initialiseHistory(g_history, MAX_NUM_HISTORY);
    sem_init(history_sem, 1, 1);

    int id;
    if ((id = listenSocket(53000)) < 0)
    {
        printf("Error connecting to socket\n");
        return 1;
    }

    int exit = 0;
    while (!exit)
    {
        sendMessage("18811931> ", id);
        printf("message sent\n");

        //Get command
        switch (getCommandFromClient(id))
        {
            case STORE:
                storeFileServer(&fl, id);
                break;
            case GET:
                getFileServer(&fl, id);
                break;
            case DELETE:
                deleteFileServer(&fl, id);
                break;
            case HISTORY:
                historyFileServer(id);
                break;
            case QUIT:
                quitServer(id);
                exit = 1;
                break;
            case INVALID:
                exit = 1;
                break;
            default:
                break;
        }
    }

    printf("Process ended");
    close(id);

    return 0;
}
