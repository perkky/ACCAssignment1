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
  
FileHistory* g_startHistory = NULL;

//The Server main function
int main(int argc, char* argv[])
{
    signal(SIGCHLD, sig_chld);

    FileList fl;
    getStorageFileList(&fl);
    for (int i = 0; i < fl.size; i++)
        printf("%s %s\n",fl.fileList[i], fl.md5List[i]);

    if (g_startHistory == NULL)
    {
        g_startHistory = (FileHistory*)malloc(sizeof(FileHistory));
        initialiseFileHistory(g_startHistory, "temp");
    }

    /*char md5sum[33];*/
    /*getMD5Sum("storage/b", md5sum);*/
    /*if (deleteFile(&fl, "d41d8cd98f00b204e9800998ecf8427e"))*/
        /*printf("File deleted\n");*/
    /*printf("%s",md5sum); */

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
