#pragma once
typedef struct FileHistory
{
    char*   id;
    char**  history;
    int     size;
    struct FileHistory* next;

} FileHistory;

void initialiseFileHistory(FileHistory* fh, char* id);
void addHistoryLine(FileHistory* list, char* time, char* id, char* type, char* ip);
void createHistoryLine(char* dest, char* time, char* id, char* type, char* ip);
void addHistory(FileHistory* list, char* id, char* history);
FileHistory* getFileHistory(FileHistory* start, char* id);
FileHistory* createNewFileHistory(FileHistory* start, char* id);
