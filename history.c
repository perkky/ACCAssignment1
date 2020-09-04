#include <stdlib.h>
#include <string.h>
#include "history.h"
#include "ACCSockets.h"
#include <stdio.h>
#include <sys/mman.h>

//Linked list specifically for history
//Could have used void pointers, but this is easier

void initialiseHistory(History* history, int maxSize)
{
    history->history = (FileHistory*)mmap(NULL, maxSize * sizeof(FileHistory), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED,-1,0);

    for (int i = 0; i < maxSize; i++)
        history->history[i].size = 0;

}


/*Old history*/

void addHistoryLine(History* history, char* time, char* id, char* type, char* ip)
{
    char line[BUFFER_SIZE];
    memset(line, 0, BUFFER_SIZE);
    createHistoryLine(line, time, id, type, ip);
    addHistory(history, id, line);
}

void createHistoryLine(char* dest, char* time, char* id, char* type, char* ip)
{
    sprintf(dest, "%s, %s, %s, %s", time, id, type, ip);
}

void addHistory(History* history, char* id, char* historyLine)
{
    FileHistory* fh = NULL;

    if ( (fh = getFileHistory(history, id)) == NULL)
    {
        //ran out of room
        if ( (fh = createNewFileHistory(history, id)) == NULL)
            return;
            
    }
    
    //Only add if there is room
    if (fh->size < MAX_HISTORY_LINES)
    {
        strcpy(fh->history[fh->size++], historyLine);
    }

}

//Loops through the fileHistory list and returns the fh that matches id
//returns NULL if not found
FileHistory* getFileHistory(History* history, char* id)
{
    FileHistory* fh = NULL;

    if (history == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < history->size; i++)
    {
        if (strcmp(history->history[i].id, id) == 0)
        {
            fh = (history->history+i);
            break;
        }
    }

    return fh;
}

//Creates new FileHistory in the list
FileHistory* createNewFileHistory(History* history, char* id)
{
    FileHistory* fh = NULL;

    if (history->size < history->maxSize)
    {
        fh = (history->history+history->size);
        strcpy(history->history[history->size++].id, id);
    }

    return fh;
}
