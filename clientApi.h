#pragma once

int storeFileClient(char* fileName, int sockfd);
int getFileClient(char* md5, char* fileName, int sockfd);
void getInput(char* arg1, char* arg2, char* arg3);