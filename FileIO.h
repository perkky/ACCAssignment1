#pragma once
#include <stdbool.h>
typedef struct FileList
{
    char** fileList;
    char** md5List;
    int size;
} FileList;


bool fileExists(FileList* fileList, char* md5);
void getFileName(FileList* fileList, char* md5, char* fileName);
bool deleteFile(FileList* fileList, char* md5);
void getMD5Sum(char* fileLocation, char* md5sum);
void getStorageFileList(FileList* fileList);
void initialiseFileList(FileList* fileList, int size);
void addStoragePrefixToFileName(char* fileName, int flag);
void toLower(char* str);