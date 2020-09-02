#include <stdlib.h>
#include <string.h>
#include "history.h"
#include "ACCSockets.h"
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

void addHistory(FileHistory* fh, char* history)
{
    fh->history = (char**)realloc(fh->history, ++fh->size * sizeof(char*));
    fh->history[fh->size -1] = (char*)malloc(BUFFER_SIZE*sizeof(char));
}

FileHistory* getFileHistory(FileHistory* start, char* id)
{
    FileHistory* fh = NULL;

    while (start->next != NULL)
    {
        if (strcmp(start->id, id) == 0)
            fh = start;

        start = start->next;

    }

    return fh;
}

FileHistory* createNewFileHistory(FileHistory* start, char* id)
{
    FileHistory* fh = (FileHistory*)malloc(sizeof(FileHistory));
    initialiseFileHistory(fh, id);

    while (start->next != NULL)
    {
        start = start->next;
    }

    start->next = fh;

    return start->next;
}
