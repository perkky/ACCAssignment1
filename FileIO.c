#include "FileIO.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#define STR_SIZE 512
const int MD5SIZE = 32;
const int MAX_FILE_SIZE = 256;

void getRandomFileName(char* dest, int len)
{
    static const char alphanum[] = "0123456789";
    strcpy(dest,"file_");

    for (int i = 0; i < len; i++)
    {
        *(dest+5+i) = alphanum[rand() % sizeof(alphanum)-1];
    }

}

//See if a file exists
//Returns 1 if it does, 0 if it doesnt
int fileExistsClient(char* fileName)
{
    FILE* f;
    if (f = fopen(fileName, "r"))
    {
        fclose(f);
        return 1;
    }

    return 0;
}

void toLower(char* str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        str[i] = tolower(str[i]);
    }
}

void getFileName(FileList* fileList, char* md5, char* fileName)
{
    for (int i = 0; i < fileList->size; i++)
    {
        if (strncmp(md5, fileList->md5List[i],MD5SIZE) == 0)
            strcpy(fileName, fileList->fileList[i]);
    }
}

bool fileExists(FileList* fileList, char* md5)
{
    for (int i = 0; i < fileList->size; i++)
    {
        if (strncmp(md5, fileList->md5List[i],MD5SIZE) == 0)
            return true;
    }

    return false;
}

bool fileExistsName(FileList* fileList, char* fileName)
{
    for (int i = 0; i < fileList->size; i++)
    {
        if (strcmp(fileName, fileList->fileList[i]) == 0)
            return true;
    }

    return false;
}

bool deleteFile(FileList* fileList, char* md5)
{
    char fileName[MAX_FILE_SIZE];
    memset(fileName, 0, sizeof(fileName)); 

    for (int i = 0; i < fileList->size; i++)
    {
        if (strncmp(md5, fileList->md5List[i],MD5SIZE) == 0)
        {
            strcpy(fileName, fileList->fileList[i]);
            addStoragePrefixToFileName(fileName, 0);
            printf("%s\n",fileName);
            remove(fileName);
            return true;
        }
    }

    return false;
}

void getMD5Sum(char* fileLocation, char* md5sum)
{
    char arg[256] = "md5sum ";
    strcat(arg, fileLocation);
    FILE* p = popen(arg, "r");

    if (p == NULL)
    {
        md5sum = NULL;
    }
    else
    {
        fgets(md5sum,MD5SIZE+1,p);

        pclose(p);
    }
}
                /*strcat(fileLocation, "storage/'");*/
                /*strcat(fileLocation, dir->d_name);*/
                /*strcat(fileLocation, "'");*/

void getStorageFileList(FileList* fileList)
{
    //Get the number of files
    int numOfFiles = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir("storage");
    if (d)
        while((dir = readdir(d)) != NULL)
            if (dir->d_type == DT_REG)
                numOfFiles++;
    closedir(d);

    initialiseFileList(fileList, numOfFiles);

    //copy the file names
    int i = 0;
    char md5sum[MD5SIZE+1];
    char fileLocation[256];
    d = opendir("storage");
    if (d)
        while((dir = readdir(d)) != NULL)
            if (dir->d_type == DT_REG)
            {
                strcpy(fileList->fileList[i], dir->d_name);
                
                strcpy(fileLocation, dir->d_name);
                addStoragePrefixToFileName(fileLocation, 1);

                getMD5Sum(fileLocation,md5sum);
                strcpy(fileList->md5List[i++], md5sum);
                memset(fileLocation, 0, sizeof(fileLocation)); 
                memset(md5sum, 0, sizeof(md5sum)); 

            }
    closedir(d);
}

void initialiseFileList(FileList* fileList, int size)
{
    fileList->fileList = (char**)malloc(size*sizeof(char*));
    fileList->md5List = (char**)malloc(size*sizeof(char*));
    fileList->size = size;
    for (int i = 0; i < size; i++)
    {
        fileList->fileList[i] = (char*)malloc(MAX_FILE_SIZE*sizeof(char));
        fileList->md5List[i] = (char*)malloc((MD5SIZE+1)*sizeof(char)); 
    }
}

void freeFileList(FileList* fileList)
{
    for (int i = 0; i < fileList->size; i++)
    {
        free(fileList->fileList[i]);
        free(fileList->md5List[i]); 
    }

    free(fileList->fileList);
    free(fileList->md5List); 
}

//Add the "storage/" prefix to the file name
//Flag - 0 to get the raw name, 1 to get it encapsulated in ' '
void addStoragePrefixToFileName(char* fileName, int flag)
{
    char temp[MAX_FILE_SIZE];
    memset(temp, 0, sizeof(temp)); 

    strcat(temp, "storage/");
    if (flag == 1)
        strcat(temp, "'");
    strcat(temp, fileName);
    if (flag == 1)
        strcat(temp, "'");
   
    memset(fileName, 0, sizeof(fileName));
    strcpy(fileName, temp);
}
