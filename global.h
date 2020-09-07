#pragma once
#include "history.h"
#include "IpList.h"
#include <semaphore.h>
//Global variables

#define BUFFER_SIZE 256
#define MAX_HISTORY_LINES 256
#define MAX_NUM_HISTORY 256
#define PORT 50070
extern History* g_history;
extern IpList* g_ipList;
extern sem_t* history_sem;
extern sem_t* ipList_sem;
