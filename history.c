#include <stdlib.h>
#include <string.h>
#include "history.h"
#include "ACCSockets.h"
#include <stdio.h>

//Linked list specifically for history
//Could have used void pointers, but this is easier
void initialiseFileHistory(FileHistory* fh, char* id)
{
    fh->id = (char*)malloc(BUFFER_SIZE*sizeof(char));
    strcpy(fh->id, id);
    fh->size = 0;
    fh->history = NULL;
    fh->next = NULL;
}

void addHistoryLine(FileHistory* list, char* time, char* id, char* type, char* ip)
{
    char line[BUFFER_SIZE];
    memset(line, 0, BUFFER_SIZE);
    createHistoryLine(line, time, id, type, ip);
    addHistory(list, id, line);
}

void createHistoryLine(char* dest, char* time, char* id, char* type, char* ip)
{
    sprintf(dest, "%s, %s, %s, %s", time, id, type, ip);
}

void addHistory(FileHistory* list, char* id, char* history)
{
    FileHistory* fh = NULL;

    if ( (fh = getFileHistory(list, id)) == NULL)
    {
        fh = createNewFileHistory(list, id);
    }

    fh->history = (char**)realloc(fh->history, ++fh->size * sizeof(char*));
    fh->history[fh->size -1] = (char*)malloc(BUFFER_SIZE*sizeof(char));
    strcpy(fh->history[fh->size-1], history);
}

//Loops through the fileHistory list and returns the fh that matches id
//returns NULL if not found
FileHistory* getFileHistory(FileHistory* start, char* id)
{
    FileHistory* fh = NULL;

    if (start == NULL)
    {
        return NULL;
    }

    do
    {
        if (strcmp(start->id, id) == 0)
            fh = start;
    }
    while ( (start = start->next) != NULL);

    return fh;
}

//Creates new FileHistory in the list
FileHistory* createNewFileHistory(FileHistory* start, char* id)
{
    FileHistory* fh = (FileHistory*)malloc(sizeof(FileHistory));
    initialiseFileHistory(fh, id);

    if (start == NULL)
        start = fh;
    else
    {
        while (start->next != NULL)
        {
            start = start->next;
        }

        start->next = fh;
    }

    return fh;
}
