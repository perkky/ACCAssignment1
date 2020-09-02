
typedef struct FileHistory
{
    char*   id;
    char**  history;
    int     size;
    struct FileHistory* next;

} FileHistory;

void initialiseFileHistory(FileHistory* fh, char* id);
void addHistory(FileHistory* fh, char* history);
FileHistory* getFileHistory(FileHistory* start, char* id);
FileHistory* createNewFileHistory(FileHistory* start, char* id);
