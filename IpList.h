#pragma once
#define BUFFER_SIZE 256
#define MAX_IP_SIZE 256

/* Contains the list of IP's that are banned, as well as the time they were banned */
typedef struct IpList
{
    char ipList[MAX_IP_SIZE][BUFFER_SIZE];
    int time[MAX_IP_SIZE];
    int size;
    int timeoutLength;
} IpList;

void initialiseIpList(IpList* ipList, int timeoutLength);
void addIp(IpList* ipList, char* ip, int time);
void checkIp(IpList* ipList, int time);
int isIpBanned(IpList* ipList, char* ip, int time);

