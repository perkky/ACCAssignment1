#pragma once
#include <stdbool.h>

/* Contains the FileList, with the corresponding md5 for each file */
typedef struct FileList
{
    char** fileList;
    char** md5List;
    int size;
} FileList;


int fileExistsClient(char* fileName);
void getRandomFileName(char* dest, int len);
bool fileExists(FileList* fileList, char* md5);
bool fileExistsName(FileList* fileList, char* fileName);
int getFileName(FileList* fileList, char* md5, char* fileName);
bool deleteFile(FileList* fileList, char* md5);
void getMD5Sum(char* fileLocation, char* md5sum);
void getStorageFileList(FileList* fileList);
void initialiseFileList(FileList* fileList, int size);
void freeFileList(FileList* fileList);
void addStoragePrefixToFileName(char* fileName, int flag);
void toLower(char* str);
