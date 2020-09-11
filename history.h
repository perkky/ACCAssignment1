#pragma once
#define BUFFER_SIZE 256
#define MAX_HISTORY_LINES 256

/* Contains the history of a single file. Can hold a max of MAX_HISTORY_LINES
 * history lines.*/
typedef struct FileHistory
{
    char    id[BUFFER_SIZE];
    char    history[MAX_HISTORY_LINES][BUFFER_SIZE];
    int     size;
} FileHistory;

/* Structure that contains all the FileHistorys for each file */
typedef struct History
{
    FileHistory*    history;
    int             size;
    int             maxSize;
} History;

void initialiseHistory(History* history, int maxSize);
void addHistoryLine(History* history, char* time, char* id, char* type, char* ip);
void createHistoryLine(char* dest, char* time, char* id, char* type, char* ip);
void addHistory(History* history, char* id, char* historyLine);
FileHistory* getFileHistory(History* history, char* id);
FileHistory* createNewFileHistory(History* history, char* id);
