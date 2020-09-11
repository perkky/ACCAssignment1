#include "IpList.h"
#include <string.h>

/* Initialise an IpList struct*/
void initialiseIpList(IpList* ipList, int timeoutLength)
{
    ipList->size = 0;
    ipList->timeoutLength = timeoutLength;
}

/* Add an Ip that needs to be blocked */
void addIp(IpList* ipList, char* ip, int time)
{
    if (ipList->size < MAX_IP_SIZE)
    {
        strcpy(ipList->ipList[ipList->size], ip);
        ipList->time[ipList->size++] = time;
    } 
}

/* Shuffle down ip. When an Ip is removed from the IpLIst, it must be shuffled down
 * to create room for more ips.*/
void shuffleDown(IpList* ipList, int idx)
{
    //if you want to delete the last element
    if (idx == ipList->size -1)
    {
        ipList->size--;
        return;
    }

    for (int i = idx; i < ipList->size -1; i++)
    {
        //replace i with i+1
        strcpy(ipList->ipList[i+1], ipList->ipList[i]);
        ipList->time[i] = ipList->time[i+1];

    }

    ipList->size--;

}

/* Check every ip to see if the timeout time has expired and remove it if it has. */
void checkIp(IpList* ipList, int time)
{
    for (int i = 0; i < ipList->size; i++)
    {
        if (time - ipList->time[i] >= ipList->timeoutLength)
        {
            //remove it and shuffle the list down
            shuffleDown(ipList, i);
            //i must stay the same as it has now been replaced
            i--;
        }
    }
}

/* Check if an ip is banned or not. Calling this also calls checkIp() to check every ip. */
int isIpBanned(IpList* ipList, char* ip, int time)
{
    checkIp(ipList, time);

    for (int i = 0; i < ipList->size; i++)
    {
        if (strcmp(ip, ipList->ipList[i]) == 0)
            return 1;

    }

    return 0;
}
