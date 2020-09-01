#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "ACCSockets.h"
#include "FileIO.h"
#include <unistd.h>
#include "serverApi.h"

//The Server main function
int main(int argc, char* argv[])
{
    FileList fl;
    getStorageFileList(&fl);
    for (int i = 0; i < fl.size; i++)
        printf("%s %s\n",fl.fileList[i], fl.md5List[i]);

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
                break;
            case QUIT:
                quitServer(id);
                exit = 1;
                break;
            case INVALID:
                break;
            default:
                break;
        }
    }

    close(id);

    return 0;
}
